#pragma once

#include <SDL.h>
#include <memory>
#include <functional>
#include <mutex>

#include "Texture2D.h"
#include "RenderTarget.h"
#include "Singleton.h"

#define RENDER RenderManager::GetInstance()

enum class eRenderAlignMode : Uint8
{
	topLeft = 0b00,
	topRight = 0b01,
	bottomLeft = 0b10,
	bottomRight = 0b11,
	top = 4,
	bottom = 5,
	right = 6,
	left = 7,
	centered = 8
};

class RenderManager final : public Singleton<RenderManager>
{

	friend class Singleton<RenderManager>;

public:

	void Init(SDL_Window* window);
	void Render();
	void Destroy();

	//void RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pivot = {0.5f,0.5f}, const SDL_FRect* srcRect = nullptr) const;
	//void RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos, const glm::vec2& pivot = { 0.5f,0.5f }, const SDL_FRect* srcRect = nullptr) const;
	//void RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos, const glm::vec2& scale, const glm::vec2& pivot = { 0.5f,0.5f }, const SDL_FRect* srcRect = nullptr) const;
	void RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos = {0.f,0.f}, const glm::vec2& scale = { 1.f,1.f }, float rotation = 0, const glm::vec2& pivot = { 0.5f,0.5f }, const SDL_FRect* srcRect = nullptr, int renderLayer = -1) const;

	//void RenderTexture(const std::shared_ptr<RenderTarget>& texture, const glm::vec2& pivot = { 0.5f,0.5f }, const SDL_FRect* srcRect = nullptr) const;
	//void RenderTexture(const std::shared_ptr<RenderTarget>& texture, const glm::vec2& pos, const glm::vec2& pivot = { 0.5f,0.5f }, const SDL_FRect* srcRect = nullptr) const;
	//void RenderTexture(const std::shared_ptr<RenderTarget>& texture, const glm::vec2& pos, const glm::vec2& scale, const glm::vec2& pivot = { 0.5f,0.5f }, const SDL_FRect* srcRect = nullptr) const;
	void RenderTexture(const std::shared_ptr<RenderTarget>& texture, const glm::vec2& pos = { 0.f,0.f }, const glm::vec2& scale = { 1.f,1.f }, float rotation = 0, const glm::vec2& pivot = { 0.5f,0.5f }, const SDL_FRect* srcRect = nullptr, int renderLayer = -1) const;


	void SetRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget) const;

	void SetRenderTargetScreen() const;

	size_t GetRenderLayersAmount() const { return m_RenderLayers.size(); }

	std::mutex& GetOpenGlMutex() { return m_OpenGlLock; }

private:

	void RenderTexture(GLuint glId, int width, int height, const glm::vec2& pos = { 0.f,0.f }, const glm::vec2& scale = { 1.f,1.f }, float rotation = 0, const glm::vec2& pivot = { 0.5f,0.5f }, const SDL_FRect* srcRect = nullptr, int renderLayer = -1) const;
	
	void RenderImGuiGameWindow() const;
	void RenderImGuiRenderInfo();
	void RenderHitboxes() const;

	virtual ~RenderManager() = default;
	RenderManager() = default;
	
	SDL_Window* m_Window{};
	int m_WindowResWidth{};
	int m_WindowResHeight{};
	int m_GameResWidth{};
	int m_GameResHeight{};

	SDL_GLContext m_pContext{};

	std::vector<std::shared_ptr<RenderTarget>> m_RenderLayers;

	std::shared_ptr<RenderTarget> m_ImGuiRenderTarget;

	std::mutex m_OpenGlLock;

	bool m_bKeepAspectRatio{ true };
	bool m_bShowHitboxes{ false };

public:

	/** STATIC FUNCTIONS */
	static void SetColor(const SDL_Color& color);
	static void RenderPoint(const glm::vec2& pos, float pointSize = 1, SDL_Color* pColor = nullptr);
	static void RenderLine(const glm::vec2& begin, const glm::vec2& end, float thickness = 1, SDL_Color* pColor = nullptr);
	static void RenderRect(const SDL_FRect& rect, bool filled = false, SDL_Color* pColor = nullptr);
	static void RenderPolygon(const std::vector<glm::vec2>& points, bool filled = false, SDL_Color* pColor = nullptr);
	static void RenderPolygon(const glm::vec2* points, size_t size, bool filled = false, SDL_Color* pColor = nullptr);
	static void RenderEllipse(const glm::vec2& center, const glm::vec2& radii, bool filled = false, SDL_Color* pColor = nullptr);

};

