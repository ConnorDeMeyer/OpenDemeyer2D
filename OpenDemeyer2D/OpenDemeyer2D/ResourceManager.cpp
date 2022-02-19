#include "OD2pch.h"
#include "ResourceManager.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

#include "RenderManager.h"
#include "Texture2D.h"
#include "Font.h"

void ResourceManager::Init(const std::string& dataPath)
{

	m_DataPath = dataPath;

	// load support for png and jpg, this takes a while!

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		throw std::runtime_error(std::string("Failed to load support for png's: ") + SDL_GetError());
	}

	if ((IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) != IMG_INIT_JPG)
	{
		throw std::runtime_error(std::string("Failed to load support for jpg's: ") + SDL_GetError());
	}

	if (TTF_Init() != 0)
	{
		throw std::runtime_error(std::string("Failed to load support for fonts: ") + SDL_GetError());
	}
}

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const std::string& file)
{
	// Don't load the same image if it is already loaded
	auto it = m_LoadedImages.find(file);
	if (it != m_LoadedImages.end())
	{
		if (!it->second.expired())
			return it->second.lock();
	}

	// Load the image if it isn't loaded already
	const auto fullPath = m_DataPath + file;
	auto texture = IMG_LoadTexture(RENDER.GetSDLRenderer(), fullPath.c_str());
	if (texture == nullptr)
	{
		throw std::runtime_error(std::string("Failed to load texture: ") + SDL_GetError());
	}
	auto sharedTexture = std::make_shared<Texture2D>(texture);

	// Add it to the list
	m_LoadedImages.insert(std::make_pair(file, sharedTexture));

	return sharedTexture;
}

std::shared_ptr<Font> ResourceManager::LoadFont(const std::string& file, unsigned size)
{
	auto it = m_LoadedFonts.find(std::make_pair(file, size));
	if (it != m_LoadedFonts.end())
	{
		if (!it->second.expired())
			return it->second.lock();
	}

	auto sharedFont = std::make_shared<Font>(m_DataPath + file, size);

	// Add to the list
	m_LoadedFonts.insert(std::make_pair(std::make_pair(file, size), sharedFont));

	return sharedFont;
}
