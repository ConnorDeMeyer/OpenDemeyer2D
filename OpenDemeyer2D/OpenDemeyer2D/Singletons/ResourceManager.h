#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <thread>
#include <future>
#include <mutex>
#include <functional>
#include <deque>
#include <SDL.h>
#include <cassert>
#include <iostream>

#include "UtilityFiles/Singleton.h"
#define RESOURCES ResourceManager::GetInstance()

class Texture2D;
class Font;
class RenderTarget;
class Surface2D;
class Sound;
class Music;

class ResourceManager final : public Singleton<ResourceManager>
{

	friend class Singleton<ResourceManager>;

private:

	ResourceManager() = default;
	virtual ~ResourceManager()
	{
		m_TerminateLoaderThreads = true;
	};

public:

	void Init(const std::string& dataPath);

public: //**// TEXTURE2D //**//

	std::shared_ptr<Texture2D> LoadTexture(const std::string& file);
	std::shared_ptr<Texture2D> LoadTexture(SDL_Surface* pSurface);
	std::shared_ptr<Texture2D> LoadTexture(int width, int height);

private:

	std::unordered_map<std::string, std::weak_ptr<Texture2D>> m_Texture2DFiles;

	std::mutex m_IMGLock;

public: //**// SURFACE2D //**//

	std::shared_ptr<Surface2D> LoadSurface(const std::string& file);
	std::shared_ptr<Surface2D> LoadSurface(int width, int height);
	std::future<std::shared_ptr<Surface2D>> LoadSurfaceAsync(const std::string& file);

private:

	void SurfaceLoaderThread();

	std::unordered_map<std::string, std::weak_ptr<Surface2D>> m_Surface2DFiles;
	std::deque<std::pair<std::string, std::shared_ptr<std::promise<std::shared_ptr<Surface2D>>>>> m_SurfaceLoaderQueue;
	std::mutex m_SurfaceQueueLock;
	std::jthread m_SurfaceLoaderThread;

public: //**// FONT //**//

	std::shared_ptr<Font> LoadFont(const std::string& file, uint32_t size);

private:

	//std::unordered_map<std::pair<std::string, uint32_t size>, std::weak_ptr<Font>> m_FontFiles;

public: //**// RENDER TARGET //**//

	std::shared_ptr<RenderTarget> CreateRenderTexture(int width, int height);

public: //**// SOUND //**//

	std::shared_ptr<Sound> LoadSound(const std::string& file);
	std::future<std::shared_ptr<Sound>> LoadSoundAsync(const std::string& file);

private:

	void SoundLoaderThread();

	std::unordered_map<std::string, std::weak_ptr<Sound>> m_SoundFiles;
	std::deque<std::pair<std::string, std::shared_ptr<std::promise<std::shared_ptr<Sound>>>>> m_SoundLoaderQueue;
	std::mutex m_SoundQueueLock;
	std::jthread m_SoundLoaderThread;

	std::mutex m_MixLock;

public: //**// MUSIC //**//

	std::shared_ptr<Music> LoadMusic(const std::string& file);
	std::future<std::shared_ptr<Music>> LoadMusicAsync(const std::string& file);

private:

	void MusicLoaderThread();

	std::unordered_map<std::string, std::weak_ptr<Music>> m_MusicFiles;
	std::deque<std::pair<std::string, std::shared_ptr<std::promise<std::shared_ptr<Music>>>>> m_MusicLoaderQueue;
	std::mutex m_MusicQueueLock;
	std::jthread m_MusicLoaderThread;

private:

	// FILE EXPLORER
	void LoadFilePaths();

private:
	
	bool m_TerminateLoaderThreads{};

	std::string m_DataPath;

	// TODO change this into a unordered_map. needs a special hash funtion
	std::map<std::pair<std::string, unsigned int>, std::weak_ptr<Font>> m_LoadedFonts;
};
