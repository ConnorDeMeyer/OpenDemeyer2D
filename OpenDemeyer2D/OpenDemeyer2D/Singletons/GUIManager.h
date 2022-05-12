#pragma once

#include "UtilityFiles/Singleton.h"
#include <memory>
#include <SDL.h>
#include <glm/glm.hpp>
#include <chrono>
#include <unordered_map>
#include <filesystem>

#include "ImGuiExt/FileDetailView.h"

class GameObject;
class RenderTarget;
struct Directory;
class Scene;
//class FileDetailView;

#define GUI GUIManager::GetInstance()

class GUIManager : public Singleton<GUIManager>
{
	friend class Singleton<GUIManager>;

public:

	void Init(SDL_Window* window);
	void Destroy();

	virtual ~GUIManager();

	std::shared_ptr<RenderTarget>& GetImGuiRenderTarget() { m_ImGuiRenderTarget; }

	void BeginGUI();
	void EndGUI();
	void RenderGUI();
	void RenderGUIOnGame();

	void SetSelectedObject(GameObject* pObject);

private:

	void RenderImGuiGameWindow();
	void RenderImGuiRenderInfo();
	void RenderImGuiObjectInfo();
	void RenderImGuiObjectLocation();
	void RenderImGuiMainMenu();
	void RenderImGuiDirView();
	void RenderImGuiDirViewRecursive(Directory* dir);
	void RenderImGuiFileView();
	void RenderImGuiFileDetails();
	void RenderImGuiEngineStats();
	void RenderImGuiEngineSettings();
	void RenderImGuiGameObjectRecursive(GameObject* go);
	void RenderImGuiSceneGraph();
	void RenderImGuiScene(Scene* pScene);

	void RenderHitboxes() const;

private:

	SDL_Window* m_Window{};

	// ImGui data members
	const size_t INPUT_TEXT_BUFFER_SIZE = 128;
	std::unordered_map<std::string, char*> m_SettingsInputBuffer;

	bool m_bKeepAspectRatio{ true };
	bool m_bShowHitboxes{ false };
	bool m_bSceneGraphQuickExit{ };

	int m_GameResWidth{};
	int m_GameResHeight{};

	std::vector<GameObject*> m_HitObjects{};
	std::weak_ptr<GameObject> m_pSelectedObject{};
	std::chrono::steady_clock::time_point m_LastClickTime{ std::chrono::high_resolution_clock::now() };

	std::shared_ptr<RenderTarget> m_ImGuiRenderTarget;

	Directory* m_pCurrentDirectory{};
	std::filesystem::path m_SelectedFile;
	FileDetailView* m_FileDetails{};
};

