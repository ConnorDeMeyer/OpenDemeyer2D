#include "OD2pch.h"
#include "SceneManager.h"

#include <b2_contact.h>

#include "Scene.h"
#include "imgui.h"

void SceneManager::Update(float deltaTime)
{
	m_pActiveScene->Update(deltaTime);
}

void SceneManager::Render() const
{
	m_pActiveScene->Render();
}

Scene& SceneManager::CreateScene(const std::string& name)
{
	Scene* pScene = new Scene(name);
	m_Scenes.emplace_back(pScene);
	if (!m_pActiveScene) m_pActiveScene = pScene;
	return *pScene;
}

Scene* SceneManager::GetScene(const std::string& name) const
{
	for (Scene* pScene : m_Scenes)
	{
		if (pScene->GetName() == name) return pScene;
	}

	return nullptr;
}

void SceneManager::SetActiveScene(const std::string& name)
{
	for (Scene* pScene : m_Scenes)
	{
		if (pScene->GetName() == name) SetActiveScene(pScene);
	}
}

void SceneManager::SetActiveScene(Scene* pScene)
{
	if (!pScene) return;

	m_pActiveScene = pScene;
}

void SceneManager::RenderImGui()
{
	bool isOpen{};
	ImGui::Begin("SceneGraph", &isOpen);

	if (!isOpen) {
		
		for (Scene* pScene : m_Scenes)
		{
			pScene->RenderImGui();
		}
	}

	ImGui::End();
}

void SceneManager::PhysicsStep(float timeStep, int velocityIterations, int positionIterations)
{
	b2World* physicsWorld = m_pActiveScene->GetPhysicsWorld();
	physicsWorld->Step(timeStep, velocityIterations, positionIterations);
	physicsWorld->ClearForces();
}

SceneManager::~SceneManager()
{
	for (Scene* pScene : m_Scenes)
	{
		delete pScene;
	}
}
