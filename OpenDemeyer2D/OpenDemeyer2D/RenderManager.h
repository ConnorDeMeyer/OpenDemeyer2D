#pragma once

#include <SDL.h>
#include <memory>
#include <functional>

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
	centered
};

class RenderManager final : public Singleton<RenderManager>
{

	friend class Singleton<RenderManager>;

public:

	void Init(SDL_Window* window);
	void Render() const;
	void Destroy();

	void RenderTexture(const std::shared_ptr<Texture2D>& texture, eRenderAlignMode renderMode = eRenderAlignMode::centered, const SDL_FRect* srcRect = nullptr) const;
	void RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos, eRenderAlignMode renderMode = eRenderAlignMode::centered, const SDL_FRect* srcRect = nullptr) const;
	void RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos, const glm::vec2& scale, eRenderAlignMode renderMode = eRenderAlignMode::centered, const SDL_FRect* srcRect = nullptr) const;
	void RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos, const glm::vec2& scale, float rotation, eRenderAlignMode renderMode = eRenderAlignMode::centered, const SDL_FRect* srcRect = nullptr) const;

	void SetRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget) const;

	void SetRenderTargetScreen() const;

private:

	void RenderImGuiGameWindow() const;

	virtual ~RenderManager() = default;
	RenderManager() = default;
	
	SDL_Window* m_Window{};
	int m_WindowResWidth{};
	int m_WindowResHeight{};
	int m_GameResWidth{};
	int m_GameResHeight{};

	SDL_GLContext m_pContext{};

	std::shared_ptr<RenderTarget> m_RenderTexture;

	bool m_bKeepAspectRatio{ true };

};

