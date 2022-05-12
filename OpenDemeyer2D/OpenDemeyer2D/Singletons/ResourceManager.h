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
#include <filesystem>

#include "UtilityFiles/Singleton.h"
#include "ImGuiExt/FileDetailView.h"
#define RESOURCES ResourceManager::GetInstance()

class Texture2D;
class Font;
class RenderTarget;
class Surface2D;
class Sound;
class Music;
class FileDetailView;

struct Directory
{
	Directory(const std::filesystem::path& _dirPath)
		: dirPath{ _dirPath }
	{};

	Directory* previous{};
	std::filesystem::path dirPath;
	std::vector<Directory*> Directories;
	std::vector<std::unique_ptr<FileDetailView>> Files;
};

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

	std::shared_ptr<Texture2D> LoadTexture(const std::string& file, bool keepLoaded = false);
	std::shared_ptr<Texture2D> LoadTexture(SDL_Surface* pSurface);
	std::shared_ptr<Texture2D> LoadTexture(int width, int height);

	const std::unordered_map<std::string, std::weak_ptr<Texture2D>>& GetTexture2DFiles() const { return m_Texture2DFiles; }

private:

	std::vector<std::shared_ptr<Texture2D>> m_AlwaysLoadedTextures;
	std::unordered_map<std::string, std::weak_ptr<Texture2D>> m_Texture2DFiles;

	std::mutex m_IMGLock;

public: //**// SURFACE2D //**//

	std::shared_ptr<Surface2D> LoadSurface(const std::string& file, bool keepLoaded = false);
	std::shared_ptr<Surface2D> LoadSurface(int width, int height);
	std::future<std::shared_ptr<Surface2D>> LoadSurfaceAsync(const std::string& file, bool keepLoaded = false);

	const std::unordered_map<std::string, std::weak_ptr<Surface2D>>& GetSurface2DFiles() const { return m_Surface2DFiles; }

private:

	void SurfaceLoaderThread();

	std::vector<std::shared_ptr<Surface2D>> m_AlwaysLoadedSurfaces;
	std::unordered_map<std::string, std::weak_ptr<Surface2D>> m_Surface2DFiles;
	std::deque<std::tuple<std::string, std::shared_ptr<std::promise<std::shared_ptr<Surface2D>>>, bool>> m_SurfaceLoaderQueue;
	std::mutex m_SurfaceQueueLock;
	std::jthread m_SurfaceLoaderThread;

public: //**// FONT //**//

	std::shared_ptr<Font> LoadFont(const std::string& file, uint32_t size, bool keepLoaded = false);

private:

	//std::unordered_map<std::pair<std::string, uint32_t size>, std::weak_ptr<Font>> m_FontFiles;

public: //**// RENDER TARGET //**//

	std::shared_ptr<RenderTarget> CreateRenderTexture(int width, int height);

public: //**// SOUND //**//

	std::shared_ptr<Sound> LoadSound(const std::string& file, bool keepLoaded = false);
	std::future<std::shared_ptr<Sound>> LoadSoundAsync(const std::string& file, bool keepLoaded = false);

	const std::unordered_map<std::string, std::weak_ptr<Sound>>& GetSoundFiles() const { return m_SoundFiles; }

private:

	void SoundLoaderThread();

	std::vector<std::shared_ptr<Sound>> m_AlwaysLoadedSounds;
	std::unordered_map<std::string, std::weak_ptr<Sound>> m_SoundFiles;
	std::deque<std::tuple<std::string, std::shared_ptr<std::promise<std::shared_ptr<Sound>>>, bool>> m_SoundLoaderQueue;
	std::mutex m_SoundQueueLock;
	std::jthread m_SoundLoaderThread;

	std::mutex m_MixLock;

public: //**// MUSIC //**//

	std::shared_ptr<Music> LoadMusic(const std::string& file, bool keepLoaded = false);
	std::future<std::shared_ptr<Music>> LoadMusicAsync(const std::string& file, bool keepLoaded = false);

	const std::unordered_map<std::string, std::weak_ptr<Music>>& GetMusicFiles() const { return m_MusicFiles; }

private:

	void MusicLoaderThread();

	std::vector<std::shared_ptr<Music>> m_AlwaysLoadedMusic;
	std::unordered_map<std::string, std::weak_ptr<Music>> m_MusicFiles;
	std::deque<std::tuple<std::string, std::shared_ptr<std::promise<std::shared_ptr<Music>>>, bool>> m_MusicLoaderQueue;
	std::mutex m_MusicQueueLock;
	std::jthread m_MusicLoaderThread;

public:

	Directory* GetRootDirectory() const { return m_RootDirectory; }

private:

	// FILE EXPLORER
	void LoadFilePaths();

private:
	
	bool m_TerminateLoaderThreads{};

	std::string m_DataPath;

	// TODO change this into a unordered_map. needs a special hash funtion
	std::map<std::pair<std::string, unsigned int>, std::weak_ptr<Font>> m_LoadedFonts;

	std::vector<std::unique_ptr<Directory>> m_Directories{};
	Directory* m_RootDirectory{};
};
