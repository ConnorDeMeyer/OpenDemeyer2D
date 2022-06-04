#pragma once
#include <SDL.h>
#include <filesystem>

/** Simple RAII container for the sdl texture*/
class Surface2D final
{
	friend class RenderManager;
	friend class ResourceManager;

public:
	//SDL_Texture* GetSDLTexture() const { return m_Texture; }
	Surface2D() = default;

	explicit Surface2D(SDL_Surface* pSurface)
		: m_pSurface{ pSurface }{}
	~Surface2D() { SDL_FreeSurface(m_pSurface); }

	Surface2D(const Surface2D&) = delete;
	Surface2D& operator= (const Surface2D&) = delete;

	Surface2D(Surface2D&& other) noexcept
		: m_pSurface{ other.m_pSurface }
		, m_sourceFile{ std::move(other.m_sourceFile) }
	{
		other.m_pSurface = nullptr;
	}

	Surface2D& operator= (Surface2D&& other) noexcept
	{
		m_pSurface = other.m_pSurface;
		other.m_pSurface = nullptr;
		m_sourceFile = std::move(other.m_sourceFile);
		return *this;
	}

	SDL_Surface* GetSurface() const { return m_pSurface; }

	const std::filesystem::path& GetFilePath() const { return m_sourceFile; }

	inline bool IsValid() { return m_pSurface; }
	inline operator bool() { return IsValid(); }

private:
	SDL_Surface* m_pSurface{};

	std::filesystem::path m_sourceFile{};
};