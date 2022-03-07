#include "OD2pch.h"
#include "Scene.h"

#include "GameObject.h"
#include "imgui.h"

Scene::Scene(const std::string& name)
	: m_Name{ name }
{
}

Scene::~Scene()
{
	for (size_t i{}; i < m_SceneTree.size(); ++i)
	{
		delete m_SceneTree[i];
	}
}

void Scene::Add(GameObject* pObject)
{
	if (pObject->m_pScene) throw ObjectAlreadyInASceneException();

	m_SceneTree.push_back(pObject);
	pObject->m_pScene = this;
	// TODO set m_pScene in the child objects
	pObject->BeginPlay();
}

void Scene::DestroyObject(GameObject* pObject)
{
	m_DestroyableObjects.push_back(pObject);
}

void Scene::DestroyObjectImmediately(GameObject* pObject)
{
	delete pObject;
	m_SceneTree.SwapRemove(pObject);
}

void Scene::Update(float deltaTime)
{
	for (GameObject* child : m_SceneTree)
	{
		child->Update(deltaTime);
	}

	for (GameObject* object : m_DestroyableObjects)
	{
		m_SceneTree.SwapRemove(object);
		DestroyObjectImmediately(object);
	}
}

void Scene::Render() const
{
	for (GameObject* child : m_SceneTree)
	{
		child->Render();
	}
}

void Scene::RenderImGui()
{
	if (ImGui::TreeNode(m_Name.c_str())) {

		for (size_t i{}; i < m_SceneTree.size(); ++i)
		{
			if (ImGui::TreeNode(std::string("GameObject" + std::to_string(i)).c_str())) {
				m_SceneTree[i]->RenderImGui();
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
}

void Scene::RemoveObject(GameObject* object)
{
	m_SceneTree.SwapRemove(object);
}
