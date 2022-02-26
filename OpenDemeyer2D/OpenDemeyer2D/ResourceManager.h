#pragma once

#include <vector>
#include <map>
#include <memory>
#include <string>

#include "Singleton.h"
#define RESOURCES ResourceManager::GetInstance()

class Texture2D;
class Font;
class RenderTarget;

class ResourceManager final : public Singleton<ResourceManager>
{

	friend class Singleton<ResourceManager>;

protected:

	ResourceManager() = default;
	virtual ~ResourceManager() = default;

public:

	void Init(const std::string& dataPath);

	std::shared_ptr<Texture2D> LoadTexture(const std::string& file);
	std::shared_ptr<Texture2D> LoadTexture(SDL_Surface* pSurface);
	std::shared_ptr<Texture2D> LoadTexture(int width, int height);

	std::shared_ptr<Font> LoadFont(const std::string& file, unsigned int size);

	std::shared_ptr<RenderTarget> CreateRenderTexture(int width, int height);

private:

	std::string m_DataPath;

	std::map<std::string, std::weak_ptr<Texture2D>> m_LoadedImages;

	std::map<std::pair<std::string, unsigned int>, std::weak_ptr<Font>> m_LoadedFonts;

};
