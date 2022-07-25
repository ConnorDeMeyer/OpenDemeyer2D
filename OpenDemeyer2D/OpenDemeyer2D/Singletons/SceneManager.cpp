#include "pch.h"
#include "SceneManager.h"

#include <b2_contact.h>

#include "EngineFiles/Scene.h"
#include "imgui.h"

void SceneManager::Destroy()
{
	for (Scene* pScene : m_Scenes)
	{
		delete pScene;
	}

	if (m_GameScene)
		delete m_GameScene.release();
}

void SceneManager::Update(float deltaTime)
{
	if (m_GameScene)
		m_GameScene->Update(deltaTime);

	//else if (m_pActiveScene)
	//	m_pActiveScene->Update(deltaTime);
}

void SceneManager::PreUpdate()
{
	if (m_GameScene)
		m_GameScene->PreUpdate(true);

	for (Scene* pScene : m_Scenes)
	{
		pScene->PreUpdate(false);
	}
}

void SceneManager::AfterUpdate()
{
	if (m_GameScene)
		m_GameScene->AfterUpdate();

	for (Scene* pScene : m_Scenes)
	{
		pScene->AfterUpdate();
	}
}

void SceneManager::Render() const
{
	if (m_GameScene)
		m_GameScene->Render();

	else if (m_pActiveScene)
		m_pActiveScene->Render();
}

Scene& SceneManager::CreateScene(const std::string& name)
{
	assert(std::find_if(name.begin(), name.end(), [](char c) {return isspace(c); }) == name.end());

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

void SceneManager::AddScene(Scene* pScene)
{
	if (!pScene || pScene == m_GameScene.get()) return;

	for (auto scene : m_Scenes)
	{
		if (pScene == scene)
			return; // if the scene is already in there
	}

	m_Scenes.emplace_back(pScene);
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
	if (!pScene || pScene == m_GameScene.get()) return;

	m_pActiveScene = pScene;

	for (auto scene : m_Scenes)
	{
		if (pScene == scene)
			return; // if the scene is already in there
	}

	m_Scenes.emplace_back(pScene);
}

Scene* SceneManager::GetActiveScene() const
{
	return (m_GameScene) ? m_GameScene.get() : m_pActiveScene;
}

void SceneManager::PhysicsStep(float timeStep, int velocityIterations, int positionIterations)
{
	if (auto scene{(m_GameScene ? m_GameScene.get() : m_pActiveScene)}) {
		b2World* physicsWorld = scene->GetPhysicsInterface()->GetPhysicsWorld();
		physicsWorld->Step(timeStep, velocityIterations, positionIterations);
		physicsWorld->ClearForces();
	}
}

void SceneManager::RemoveScene(Scene* pScene)
{
	for (size_t i{}; i < m_Scenes.size(); ++i)
	{
		if (pScene == m_Scenes[i])
		{
			m_Scenes[i] = m_Scenes.back();
			m_Scenes.pop_back();
			if (m_pActiveScene == pScene)
				m_pActiveScene = nullptr;
			delete pScene;
			return;
		}
	}
}

void SceneManager::RemoveAllScenes()
{
	for (auto pScene : m_Scenes)
	{
		delete pScene;
	}
	m_Scenes.clear();
}

void SceneManager::RemoveActiveScene()
{
	RemoveScene(m_pActiveScene);
}

void SceneManager::Serialize(std::ostream& os)
{
	for (auto& scene : m_Scenes)
	{
		os << "{\n";
		scene->Serialize(os);
		os << "}\n";
	}
}

void SceneManager::PlayScene(Scene* pScene)
{
	if (m_GameScene)
		return;

#ifdef _DEBUG
	Scene* sceneToPlay = pScene ? pScene : m_pActiveScene;
	if (sceneToPlay)
	{
		m_GameScene = std::unique_ptr<Scene>(new Scene("Game"));
		m_GameScene->Copy(sceneToPlay);
	}
#else
	m_GameScene = std::unique_ptr<Scene>(pScene);
#endif
}

void SceneManager::StopPlayingScene()
{
	if (m_GameScene)
	{
		m_GameScene.reset();
	}
}