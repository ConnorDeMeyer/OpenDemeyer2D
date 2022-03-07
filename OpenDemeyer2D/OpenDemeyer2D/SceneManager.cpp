#include "OD2pch.h"
#include "SceneManager.h"

#include "Scene.h"
#include "imgui.h"

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

SceneManager::~SceneManager()
{
	for (Scene* pScene : m_Scenes)
	{
		delete pScene;
	}
}
