#pragma once

#include <SDL.h>
#include <memory>

#include "Texture2D.h"
#include "Singleton.h"
#define RENDER RenderManager::GetInstance()

/**
 * RAII wrapper for the SDL renderer
 */
class RenderManager final : public Singleton<RenderManager>
{

	friend class Singleton<RenderManager>;

public:

	void Init(SDL_Window* window);
	void Render() const;
	void Destroy();

	SDL_Renderer* GetSDLRenderer() const { return m_Renderer; }
	const SDL_Color& GetBackgroundColor() const { return m_clearColor; }
	void SetBackgroundColor(const SDL_Color& color) { m_clearColor = color; }

	void RenderTexture(const std::shared_ptr<Texture2D>& texture, float x, float y) const;
	void RenderTexture(const std::shared_ptr<Texture2D>& texture, float x, float y, float width, float height) const;


private:

	virtual ~RenderManager() = default;
	RenderManager() = default;

	SDL_Renderer* m_Renderer{};
	SDL_Window* m_Window{};
	SDL_Color m_clearColor{};
};

