#pragma once

#include "UtilityFiles/Singleton.h"
#include <memory>
#include <SDL.h>
#include <glm/glm.hpp>
#include <chrono>

class GameObject;
class RenderTarget;

#define GUI GUIManager::GetInstance()

class GUIManager : public Singleton<GUIManager>
{
	friend class Singleton<GUIManager>;

public:

	void Init(SDL_Window* window);
	void Destroy();

	std::shared_ptr<RenderTarget>& GetImGuiRenderTarget() { m_ImGuiRenderTarget; }

	void BeginGUI();
	void EndGUI();
	void RenderGUI();
	void RenderGUIOnGame();

	void SetSelectedObject(GameObject* pObject);

private:

	void RenderImGuiGameWindow();
	void RenderImGuiRenderInfo();
	void RenderHitboxes() const;
	void RenderImGuiObjectInfo();
	void RenderImGuiObjectLocation();
	void RenderImGuiMainMenu();

private:

	SDL_Window* m_Window{};

	bool m_bKeepAspectRatio{ true };
	bool m_bShowHitboxes{ false };

	int m_GameResWidth{};
	int m_GameResHeight{};

	std::vector<GameObject*> m_HitObjects{};
	std::weak_ptr<GameObject> m_pSelectedObject{};
	std::chrono::steady_clock::time_point m_LastClickTime{ std::chrono::high_resolution_clock::now() };

	std::shared_ptr<RenderTarget> m_ImGuiRenderTarget;

};

