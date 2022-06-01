/**
 * Open Gl code taken from the game framework from the Programming 2 course of DAE howest
 */
#include "ResourceManager.h"

#include <stack>
#include <vector>
#include <fstream>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <gl/glew.h>

#include "RenderManager.h"
#include "ResourceWrappers/Surface2D.h"
#include "ResourceWrappers/Texture2D.h"
#include "ResourceWrappers/RenderTarget.h"
#include "ResourceWrappers/Sound.h"
#include "ResourceWrappers/Prefab.h"
#include "ResourceWrappers/Font.h"

#include "EngineFiles/GameObject.h"
#include "EngineFiles/Scene.h"

#include "ImGuiExt/FileDetailView.h"

using namespace std::filesystem;

void ResourceManager::Init(const path& dataPath)
{

	m_DataPath = dataPath;
	m_DataPath._Remove_filename_and_separator();

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

	m_SurfaceLoaderThread = std::jthread(&ResourceManager::SurfaceLoaderThread, this);
	m_SoundLoaderThread = std::jthread(&ResourceManager::SoundLoaderThread, this);
	m_MusicLoaderThread = std::jthread(&ResourceManager::MusicLoaderThread, this);

	LoadFilePaths();

}

#pragma region FileLoaders

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const path& file, bool keepLoaded)
{
	auto it = m_Texture2DFiles.find(file);
	if (it != m_Texture2DFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	SDL_Surface* pLoadedSurface{};
	{
		path finalPath = GetfinalPath(file);

		std::scoped_lock<std::mutex> lock(m_IMGLock);

		pLoadedSurface = IMG_Load(finalPath.string().c_str());
		if (!pLoadedSurface)
		{
			//throw std::runtime_error(std::string("Failed to load texture: ") + SDL_GetError());
			// TODO log error
		}
	}

	std::shared_ptr<Texture2D> texture2d = pLoadedSurface ? LoadTexture(pLoadedSurface) : nullptr;
	texture2d->m_sourceFile = file;
	m_Texture2DFiles.emplace(file, texture2d);

	if (keepLoaded)
	{
		m_AlwaysLoadedTextures.emplace_back(texture2d);
	}

	return texture2d;
}

std::shared_ptr<Font> ResourceManager::LoadFont(const path& file, unsigned size, bool)
{
	// Load the resource and lock the resource loading mechanism
	//std::scoped_lock<std::mutex> lock(*m_ResourceLoaderLocks[typeid(Font)]);

	auto it = m_LoadedFonts.find(std::make_pair(file, size));
	if (it != m_LoadedFonts.end())
	{
		if (!it->second.expired())
			return it->second.lock();
	}

	path finalPath = GetfinalPath(file);

	auto sharedFont = std::make_shared<Font>(finalPath.string().c_str(), size);

	// Add to the list
	m_LoadedFonts.insert(std::make_pair(std::make_pair(file, size), sharedFont));

	return sharedFont;
}

std::shared_ptr<Surface2D> ResourceManager::LoadSurface(const path& file, bool keepLoaded)
{
	auto it = m_Surface2DFiles.find(file);
	if (it != m_Surface2DFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	// SURFACE LOADER
	SDL_Surface* pLoadedSurface{};
	{
		path finalPath = GetfinalPath(file);

		std::scoped_lock<std::mutex> lock(m_IMGLock);

		pLoadedSurface = IMG_Load(finalPath.string().c_str());
		if (!pLoadedSurface)
		{
			//throw std::runtime_error(std::string("Failed to load surface: ") + SDL_GetError());
			// TODO log error
		}
	}

	std::shared_ptr<Surface2D> surface2D = pLoadedSurface ? std::make_shared<Surface2D>(pLoadedSurface) : nullptr;
	surface2D->m_sourceFile = file;
	m_Surface2DFiles.emplace(file, surface2D);

	if (keepLoaded)
	{
		m_AlwaysLoadedSurfaces.emplace_back(surface2D);
	}

	return surface2D;
}

std::shared_ptr<Sound> ResourceManager::LoadSound(const path& file, bool keepLoaded)
{
	auto it = m_SoundFiles.find(file);
	if (it != m_SoundFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	// SOUND LOADING
	Mix_Chunk* sample;
	{
		path finalPath = GetfinalPath(file);

		std::scoped_lock<std::mutex> lock(m_MixLock);

		sample = Mix_LoadWAV(finalPath.string().c_str());
		if (!sample) {
			//throw std::runtime_error(Mix_GetError());
			// TODO log error
		}
	}

	std::shared_ptr<Sound> sound = sample ? std::make_shared<Sound>(sample) : nullptr;
	sound->m_sourceFile = file;
	m_SoundFiles.emplace(file, sound);

	if (keepLoaded)
	{
		m_AlwaysLoadedSounds.emplace_back(sound);
	}

	return sound;
}


std::shared_ptr<Music> ResourceManager::LoadMusic(const path& file, bool keepLoaded)
{
	auto it = m_MusicFiles.find(file);
	if (it != m_MusicFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	// MUSIC LOADING
	Mix_Music* musicSample;
	{
		path finalPath = GetfinalPath(file);

		std::scoped_lock<std::mutex> lock(m_MixLock);

		musicSample = Mix_LoadMUS(finalPath.string().c_str());
		if (!musicSample) {
			//throw std::runtime_error(Mix_GetError());
			// TODO log error
		}
	}

	std::shared_ptr<Music> music = musicSample ? std::make_shared<Music>(musicSample) : nullptr;
	music->m_sourceFile = file;
	m_MusicFiles.emplace(file, music);

	if (keepLoaded)
	{
		m_AlwaysLoadedMusic.emplace_back(music);
	}

	return music;
}

std::shared_ptr<Prefab> ResourceManager::LoadPrefab(const path& file, bool keepLoaded)
{
	auto it = m_PrefabFiles.find(file);
	if (it != m_PrefabFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	// PREFAB LOADING
	std::shared_ptr<Prefab> prefab;
	{
		path finalPath = GetfinalPath(file);

		std::ifstream is(finalPath);
		Deserializer deserializer{};
		try
		{
			prefab = std::shared_ptr<Prefab>(new Prefab(deserializer.DeserializeObject(is)));
		}
		catch (std::exception&)
		{
			// TODO log error
			prefab = nullptr;
		}
	}

	prefab->m_sourceFile = file;
	m_PrefabFiles.emplace(file, prefab);

	if (keepLoaded)
	{
		m_AlwaysLoadedPrefabs.emplace_back(prefab);
	}

	return prefab;
}

#pragma endregion

#pragma region FileLoadersAsync

std::shared_ptr<Surface2D> ResourceManager::LoadSurfaceAsync(const path& file, bool keepLoaded)
{
	auto it = m_Surface2DFiles.find(file);
	if (it != m_Surface2DFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Surface2D> resource = std::make_shared_for_overwrite<Surface2D>();

	std::scoped_lock<std::mutex> lock(m_SurfaceQueueLock);

	m_SurfaceLoaderQueue.emplace_back(file, resource, keepLoaded);
	return resource;
}

std::shared_ptr<Sound> ResourceManager::LoadSoundAsync(const path& file, bool keepLoaded)
{
	auto it = m_SoundFiles.find(file);
	if (it != m_SoundFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Sound> resource = std::make_shared_for_overwrite<Sound>();

	std::scoped_lock<std::mutex> lock(m_SoundQueueLock);

	m_SoundLoaderQueue.emplace_back(file, resource, keepLoaded);
	return resource;
}

std::shared_ptr<Music> ResourceManager::LoadMusicAsync(const path& file, bool keepLoaded)
{
	auto it = m_MusicFiles.find(file);
	if (it != m_MusicFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Music> resource = std::make_shared_for_overwrite<Music>();

	std::scoped_lock<std::mutex> lock(m_MusicQueueLock);

	m_MusicLoaderQueue.emplace_back(file, resource, keepLoaded);
	return resource;
}

std::shared_ptr<Prefab> ResourceManager::LoadPrefabAsync(const std::filesystem::path& file, bool keepLoaded)
{
	auto it = m_PrefabFiles.find(file);
	if (it != m_PrefabFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::shared_ptr<Prefab> resource = std::make_shared_for_overwrite<Prefab>();

	std::scoped_lock<std::mutex> lock(m_PrefabQueueLock);

	m_PrefabLoaderQueue.emplace_back(file, resource, keepLoaded);
	return resource;
}

#pragma endregion

#pragma region ThreadedLoaders

using namespace std::chrono_literals;

void ResourceManager::SoundLoaderThread()
{
	auto& queue = m_SoundLoaderQueue;

	bool* terminate{ &m_TerminateLoaderThreads };
	std::condition_variable threadIdle;
	std::mutex waitlock;
	std::unique_lock<std::mutex> uniqueWaitLock(waitlock);

	while (true)
	{
		threadIdle.wait_for(uniqueWaitLock, 100ms, [&terminate, &queue]
			{
				return (*terminate || !queue.empty());
			});

		// terminate the thread if terminateLoaderThreads is true
		if (*terminate)
			return;

		if (queue.empty())
			continue;

		std::tuple<path, std::shared_ptr<Sound>, bool> entry;
		{
			std::scoped_lock<std::mutex> lock(m_SoundQueueLock);
			entry = queue.front();
			queue.pop_front();
		}
		std::shared_ptr<Sound> resource = LoadSound(std::get<0>(entry), std::get<2>(entry));
		std::swap(*resource, *std::get<1>(entry));
	}
}

void ResourceManager::MusicLoaderThread()
{
	auto& queue = m_MusicLoaderQueue;

	bool* terminate{ &m_TerminateLoaderThreads };
	std::condition_variable threadIdle;
	std::mutex waitlock;
	std::unique_lock<std::mutex> uniqueWaitLock(waitlock);

	while (true)
	{
		threadIdle.wait_for(uniqueWaitLock, 100ms, [&terminate, &queue]
			{
				return (*terminate || !queue.empty());
			});

		// terminate the thread if terminateLoaderThreads is true
		if (*terminate)
			return;

		if (queue.empty())
			continue;

		std::tuple<path, std::shared_ptr<Music>,bool> entry;
		{
			std::scoped_lock<std::mutex> lock(m_MusicQueueLock);
			entry = queue.front();
			queue.pop_front();
		}

		std::shared_ptr<Music> resource = LoadMusic(std::get<0>(entry), std::get<2>(entry));
		std::swap(*resource, *std::get<1>(entry));
	}
}

void ResourceManager::SurfaceLoaderThread()
{
	auto& queue = m_SurfaceLoaderQueue;

	bool* terminate{ &m_TerminateLoaderThreads };
	std::condition_variable threadIdle;
	std::mutex waitlock;
	std::unique_lock<std::mutex> uniqueWaitLock(waitlock);

	while (true)
	{
		threadIdle.wait_for(uniqueWaitLock, 100ms, [&terminate, &queue]
			{
				return (*terminate || !queue.empty());
			});

		// terminate the thread if terminateLoaderThreads is true
		if (*terminate)
			return;

		if (queue.empty())
			continue;

		std::tuple<path, std::shared_ptr<Surface2D>,bool> entry;
		{
			std::scoped_lock<std::mutex> lock(m_SurfaceQueueLock);
			entry = queue.front();
			queue.pop_front();
		}

		std::shared_ptr<Surface2D> resource = LoadSurface(std::get<0>(entry), std::get<2>(entry));
		std::swap(*resource, *std::get<1>(entry));
	}
}

void ResourceManager::PrefabLoaderThread()
{
	auto& queue = m_PrefabLoaderQueue;

	bool* terminate{ &m_TerminateLoaderThreads };
	std::condition_variable threadIdle;
	std::mutex waitlock;
	std::unique_lock<std::mutex> uniqueWaitLock(waitlock);

	while (true)
	{
		threadIdle.wait_for(uniqueWaitLock, 100ms, [&terminate, &queue]
			{
				return (*terminate || !queue.empty());
			});

		// terminate the thread if terminateLoaderThreads is true
		if (*terminate)
			return;

		if (queue.empty())
			continue;

		std::tuple<path, std::shared_ptr<Prefab>, bool> entry;
		{
			std::scoped_lock<std::mutex> lock(m_PrefabQueueLock);
			entry = queue.front();
			queue.pop_front();
		}

		std::shared_ptr<Prefab> resource = LoadPrefab(std::get<0>(entry), std::get<2>(entry));
		std::swap(*resource, *std::get<1>(entry));
	}
}

#pragma endregion

void ResourceManager::LoadFilePaths()
{
	directory_iterator it(m_DataPath);
	
	m_Directories.clear();

	// create a first directory that will be at the top of the tree
	std::unique_ptr<Directory> topDir{ new Directory(m_DataPath) };
	m_RootDirectory = topDir.get();

	// create a stack that we will use when iterating over all the directories to know which we are currently on
	// and to get the previous directory
	std::stack<Directory*, std::vector<Directory*>> directoryStack;
	directoryStack.push(m_RootDirectory);

	m_Directories.emplace_back(std::move(topDir));

	for (auto& entry : it)
	{
		m_Directories.emplace_back(new Directory(entry.path()));
		m_Directories.back().get()->previous = directoryStack.top();
		directoryStack.push(m_Directories.back().get());

		// link the first and the second directory
		m_Directories.front()->Directories.emplace_back(directoryStack.top());

		recursive_directory_iterator subDirectoryIt{ directoryStack.top()->dirPath };

		for (auto& subEntry : subDirectoryIt)
		{
			int currentDepth{ subDirectoryIt.depth() };

			if (subDirectoryIt.depth() > currentDepth) // we went a directory deeper
			{
				directoryStack.push(m_Directories.back().get());
			}
			else if (subDirectoryIt.depth() < currentDepth) // we went back a directory
			{
				directoryStack.pop();
			}
			currentDepth = subDirectoryIt.depth();

			if (subEntry.is_directory())
			{
				// if current item in the iterator is a directory we add it to the Vector
				m_Directories.emplace_back(new Directory(subEntry.path()));
				m_Directories.back().get()->previous = directoryStack.top();
				directoryStack.top()->Directories.emplace_back(m_Directories.back().get());
			}
			else if (subEntry.is_regular_file())
			{
				// we add the file to the current directory (top of the stack)
				directoryStack.top()->Files.emplace_back(FileDetailView::FileDetailFactory(subEntry.path()));
			}
		}
	}
}

path ResourceManager::GetfinalPath(const std::filesystem::path& inputPath)
{
	path finalPath{};

	if (!inputPath.is_absolute() && (*inputPath.begin() != m_DataPath))
		finalPath = m_DataPath / inputPath;
	else
		finalPath = inputPath;

	return finalPath;
}

void ResourceManager::DeleteDirectory(Directory* dir)
{
	remove_all(dir->dirPath);

	m_Directories.erase(std::find_if(m_Directories.begin(), m_Directories.end(), [dir](const std::unique_ptr<Directory>& pDir) {return pDir.get() == dir; }));
}

void ResourceManager::AddDirectory(Directory* root, const std::string& DirName)
{
	auto newDir = root->dirPath;
	newDir /= DirName;
	create_directory(newDir);

	m_Directories.emplace_back(new Directory(newDir));
	Directory* newDirectory = m_Directories.back().get();
	for (auto& dir : m_Directories)
	{
		if (dir.get() == root)
		{
			dir->Directories.push_back(newDirectory);
			newDirectory->previous = dir.get();
			break;
		}
	}
}

void ResourceManager::SaveGameObject(GameObject* pGameObject, path& outPutPath)
{
	if (outPutPath.extension() != ".gobj")
		outPutPath.replace_extension("");
	if (!outPutPath.has_extension())
		outPutPath += ".gobj";

	auto of = std::ofstream(outPutPath);

	pGameObject->Serialize(of);

	auto newGo = new GameObject();
	newGo->Copy(pGameObject);

	auto dir = GetDirectory(outPutPath);
	dir->Files.emplace_back(new PrefabDetailView(outPutPath, std::shared_ptr<Prefab>(new Prefab(newGo))));
}

void ResourceManager::SaveScene(Scene* pScene, path& outPutPath)
{
	if (outPutPath.extension() != ".scene")
		outPutPath.replace_extension("");
	if (!outPutPath.has_extension())
		outPutPath += ".scene";

	auto of = std::ofstream(outPutPath);

	pScene->Serialize(of);

	auto dir = GetDirectory(outPutPath);
	dir->Files.emplace_back(new SceneDetailView(outPutPath, pScene));
}

Directory* ResourceManager::GetDirectory(const path& path)
{
	auto parentPath = path.parent_path();

	for (auto& dir : m_Directories)
	{
		if (dir->dirPath == parentPath)
		{
			return dir.get();
		}
	}

	return nullptr;
}

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(SDL_Surface* pSurface)
{
	// Get Image Size
	int width = pSurface->w;
	int height = pSurface->h;
	GLuint id;

	// Get Pixel Format
	GLenum pixelFormat{ GL_RGB };
	switch (pSurface->format->BytesPerPixel)
	{
	case 3:
		if (pSurface->format->Rmask == 0x000000ff)
			pixelFormat = GL_RGB;
		else
			pixelFormat = GL_BGR_EXT;
		break;
	case 4:
		if (pSurface->format->Rmask == 0x000000ff)
			pixelFormat = GL_RGBA;
		else
			pixelFormat = GL_BGRA_EXT;
		break;
	default:
		throw std::runtime_error("Texture::CreateFromSurface, unknown pixel format");
	}

	//Generate an array of textures.  We only want one texture (one element array), so trick
	//it by treating "texture" as array of length one.
	glGenTextures(1, &id);

	// Select(bind) the texture we just generated as the current 2D texture OpenGL is using / modifying.
	//All subsequent changes to OpenGL's texturing state for 2D textures will affect this texture.
	glBindTexture(GL_TEXTURE_2D, id);
	// check for errors. Can happen if a texture is created while a static pointer is being initialized, even before the call to the main function.
	GLenum e = glGetError();
	if (e != GL_NO_ERROR)
	{
		std::cerr << "Texture::CreateFromSurface, error binding textures, Error id = " << e << '\n';
		std::cerr << "Can happen if a texture is created before performing the initialization code (e.g. a static Texture object).\n";
		std::cerr << "There might be a white rectangle instead of the image.\n";
	}

	// Specify the texture's data.  
	// This function is a bit tricky, and it's hard to find helpful documentation. 
	// A summary:
	//    GL_TEXTURE_2D:    The currently bound 2D texture (i.e. the one we just made)
	//                0:    The mipmap level.  0, since we want to update the base level mipmap image (i.e., the image itself,
	//                         not cached smaller copies)
	//          GL_RGBA:    Specifies the number of color components in the texture.
	//                     This is how OpenGL will store the texture internally (kinda)--
	//                     It's essentially the texture's type.
	//       surface->w:    The width of the texture
	//       surface->h:    The height of the texture
	//                0:    The border.  Don't worry about this if you're just starting.
	//      pixelFormat:    The format that the *data* is in--NOT the texture! 
	// GL_UNSIGNED_BYTE:    The type the data is in.  In SDL, the data is stored as an array of bytes, with each channel
	//                         getting one byte.  This is fairly typical--it means that the image can store, for each channel,
	//                         any value that fits in one byte (so 0 through 255).  These values are to be interpreted as
	//                         *unsigned* values (since 0x00 should be dark and 0xFF should be bright).
	//  surface->pixels:    The actual data.  As above, SDL's array of bytes.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pSurface->w, pSurface->h, 0, pixelFormat, GL_UNSIGNED_BYTE, pSurface->pixels);

	// Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
	// *smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
	// each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
	// further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
	// them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return std::make_shared<Texture2D>(id, width, height);
}

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(int width, int height)
{

	SDL_Surface* pSurface{};
	pSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

	auto texture = LoadTexture(pSurface);

	SDL_FreeSurface(pSurface);

	return texture;
}

std::shared_ptr<RenderTarget> ResourceManager::CreateRenderTexture(int width, int height)
{
	//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/

	GLuint frameBuffer{};
	GLuint renderedTexture{};

	// Make framebuffer which is going to be the render target
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// Make the texture that will output the pixels
	glGenTextures(1, &renderedTexture);

	// Bind the created texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Give an empty image to OpenGl
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set m_RenderedTexture as our color attachment #0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << glewGetErrorString(status);
		throw std::runtime_error("Was unable to create the render texture");
	}

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<RenderTarget>(frameBuffer, renderedTexture, width, height);
}

std::shared_ptr<Surface2D> ResourceManager::LoadSurface(int width, int height)
{
	SDL_Surface* pSurface = SDL_CreateRGBSurfaceWithFormat(0,width,height,32,SDL_PIXELFORMAT_RGBA32);
	return std::make_shared<Surface2D>(pSurface);
}


