#include "SceneManager.h"

#include "Scene.h"

void SceneManager::Update(float deltaTime)
{
	for (Scene* pScene : m_Scenes)
	{
		pScene->Update(deltaTime);
	}
}

void SceneManager::Render() const
{
	for (Scene* pScene : m_Scenes)
	{
		pScene->Render();
	}
}

Scene& SceneManager::CreateScene(const std::string& name)
{
	Scene* pScene = new Scene(name);
	m_Scenes.push_back(pScene);
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

SceneManager::~SceneManager()
{
	for (Scene* pScene : m_Scenes)
	{
		delete pScene;
	}
}
