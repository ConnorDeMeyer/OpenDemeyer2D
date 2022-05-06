#pragma once
#include <SDL.h>
#include <gl/glew.h>
#include <string>

/** Simple RAII container for the sdl texture*/
class Surface2D final
{
	friend class RenderManager;
	friend class ResourceManager;

public:
	//SDL_Texture* GetSDLTexture() const { return m_Texture; }

	explicit Surface2D(SDL_Surface* pSurface)
		: m_pSurface{ pSurface }{}
	~Surface2D() { SDL_FreeSurface(m_pSurface); }

	Surface2D(const Surface2D&) = delete;
	Surface2D(Surface2D&&) = delete;
	Surface2D& operator= (const Surface2D&) = delete;
	Surface2D& operator= (const Surface2D&&) = delete;

	SDL_Surface* GetSurface() const { return m_pSurface; }

	const std::string& GetFilePath() const { return m_sourceFile; }


private:
	SDL_Surface* m_pSurface{};

	std::string m_sourceFile{};
};