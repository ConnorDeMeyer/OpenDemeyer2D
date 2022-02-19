#include "Scene.h"

#include "GameObject.h"

Scene::Scene(const std::string& name)
	: m_Name{ name }
{
}

Scene::~Scene()
{
	for (GameObject* object : m_SceneTree)
		delete object;
}

void Scene::Add(GameObject* pObject)
{
	if (pObject->m_pScene) throw ObjectAlreadyInASceneException();

	m_SceneTree.push_back(pObject);
	pObject->m_pScene = this;
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

void Scene::RemoveObject(GameObject* object)
{
	m_SceneTree.SwapRemove(object);
}
