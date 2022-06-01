#pragma once
#include "Font.h"
#include <SDL_ttf.h>
#include <stdexcept>
#include <filesystem>

struct _TTF_Font;

/**
 * Simple RAII wrapper for an _TTF_Font
 */
class Font final
{
public:
	_TTF_Font* GetFont() const { return m_Font; }

	explicit Font(const std::string& fullPath, unsigned int size)
	{
		m_Font = TTF_OpenFont(fullPath.c_str(), size);
		if (m_Font == nullptr)
		{
			throw std::runtime_error(std::string("Failed to load font: ") + SDL_GetError());
		}
		m_Size = size;
		m_Path = fullPath;
	}

	unsigned int GetSize() const { return m_Size; }

	const std::filesystem::path& GetPath() const { return m_Path; }

	~Font()
	{
		TTF_CloseFont(m_Font);
	}

	Font(const Font &) = delete;
	Font(Font &&) = delete;
	Font & operator= (const Font &) = delete;
	Font & operator= (const Font &&) = delete;
private:
	_TTF_Font* m_Font;
	unsigned int m_Size;
	std::filesystem::path m_Path;
};