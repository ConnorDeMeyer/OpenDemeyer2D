#pragma once

#include <SDL.h>
#include <memory>
#include <functional>

#include "Texture2D.h"
#include "RenderTarget.h"
#include "Singleton.h"

#define RENDER RenderManager::GetInstance()

enum class RenderAlignMode : Uint8
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

	void RenderTexture(const std::shared_ptr<Texture2D>& texture, RenderAlignMode renderMode = RenderAlignMode::centered,  SDL_FRect* srcRect = nullptr) const;
	void RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos, RenderAlignMode renderMode = RenderAlignMode::centered, SDL_FRect* srcRect = nullptr) const;
	void RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos, const glm::vec2& scale, RenderAlignMode renderMode = RenderAlignMode::centered, SDL_FRect* srcRect = nullptr) const;
	void RenderTexture(const std::shared_ptr<Texture2D>& texture, const glm::vec2& pos, const glm::vec2& scale, float rotation, RenderAlignMode renderMode = RenderAlignMode::centered, SDL_FRect* srcRect = nullptr) const;

	//void SetResolution(int w, int h);

	void SetRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget) const;

	void SetRenderTargetScreen() const;

private:

	void RenderImGuiGameWindow() const;

	//void RenderImGuiImages() const;

	virtual ~RenderManager() = default;
	RenderManager() = default;
	
	SDL_Window* m_Window{};
	int windowResWidth{};
	int windowResHeight{};
	//SDL_Color m_clearColor{};

	SDL_GLContext m_pContext{};

	struct imGuiRenderParameters
	{
		SDL_FRect dst;
		SDL_Texture* tex;
	};

	std::shared_ptr<RenderTarget> m_RenderTexture;

	

};

