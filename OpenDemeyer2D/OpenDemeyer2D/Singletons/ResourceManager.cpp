/**
 * Open Gl code taken from the game framework from the Programming 2 course of DAE howest
 */
#include "ResourceManager.h"

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
#include "ResourceWrappers/Font.h"

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

	m_SurfaceLoaderThread = std::jthread(&ResourceManager::SurfaceLoaderThread, this);
	m_SoundLoaderThread = std::jthread(&ResourceManager::SoundLoaderThread, this);
	m_MusicLoaderThread = std::jthread(&ResourceManager::MusicLoaderThread, this);

	LoadFilePaths();

}

#pragma region FileLoaders

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const std::string& file)
{
	auto it = m_Texture2DFiles.find(file);
	if (it != m_Texture2DFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	std::string fullPath = m_DataPath + file;
	SDL_Surface* pLoadedSurface{};
	{
		std::scoped_lock<std::mutex> lock(m_IMGLock);

		pLoadedSurface = IMG_Load(fullPath.c_str());
		if (!pLoadedSurface)
		{
			throw std::runtime_error(std::string("Failed to load texture: ") + SDL_GetError());
		}
	}

	auto texture2d = LoadTexture(pLoadedSurface);
	texture2d->m_sourceFile = file;
	m_Texture2DFiles.emplace(file, texture2d);

	return texture2d;
}

std::shared_ptr<Font> ResourceManager::LoadFont(const std::string& file, unsigned size)
{
	// Load the resource and lock the resource loading mechanism
	//std::scoped_lock<std::mutex> lock(*m_ResourceLoaderLocks[typeid(Font)]);

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

std::shared_ptr<Surface2D> ResourceManager::LoadSurface(const std::string& file)
{
	auto it = m_Surface2DFiles.find(file);
	if (it != m_Surface2DFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	// SURFACE LOADER
	std::string fullPath = m_DataPath + file;
	SDL_Surface* pLoadedSurface{};
	{
		std::scoped_lock<std::mutex> lock(m_IMGLock);

		pLoadedSurface = IMG_Load(fullPath.c_str());
		if (!pLoadedSurface)
		{
			throw std::runtime_error(std::string("Failed to load surface: ") + SDL_GetError());
		}
	}

	auto surface2D = std::make_shared<Surface2D>(pLoadedSurface);
	surface2D->m_sourceFile = file;
	m_Surface2DFiles.emplace(file, surface2D);

	return surface2D;
}

std::shared_ptr<Sound> ResourceManager::LoadSound(const std::string& file)
{
	auto it = m_SoundFiles.find(file);
	if (it != m_SoundFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	// SOUND LOADING
	Mix_Chunk* sample;
	std::string fullPath = m_DataPath + file;
	{
		std::scoped_lock<std::mutex> lock(m_MixLock);

		sample = Mix_LoadWAV(fullPath.c_str());
		if (!sample) {
			throw std::runtime_error(Mix_GetError());
		}
	}

	auto sound = std::make_shared<Sound>(sample);
	sound->m_sourceFile = file;
	m_SoundFiles.emplace(file, sound);

	return sound;
}


std::shared_ptr<Music> ResourceManager::LoadMusic(const std::string& file)
{
	auto it = m_MusicFiles.find(file);
	if (it != m_MusicFiles.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	// MUSIC LOADING
	Mix_Music* musicSample;
	std::string fullPath = m_DataPath + file;
	{
		std::scoped_lock<std::mutex> lock(m_MixLock);

		musicSample = Mix_LoadMUS(fullPath.c_str());
		if (!musicSample) {
			throw std::runtime_error(Mix_GetError());
		}
	}

	auto music = std::make_shared<Music>(musicSample);
	music->m_sourceFile = file;
	m_MusicFiles.emplace(file, music);

	return music;
}

#pragma endregion

#pragma region FileLoadersAsync

std::future<std::shared_ptr<Surface2D>> ResourceManager::LoadSurfaceAsync(const std::string& file)
{
	std::scoped_lock<std::mutex> lock(m_SurfaceQueueLock);

	m_SurfaceLoaderQueue.emplace_back(file, new std::promise<std::shared_ptr<Surface2D>>());
	return m_SurfaceLoaderQueue.back().second->get_future();
}

std::future<std::shared_ptr<Sound>> ResourceManager::LoadSoundAsync(const std::string& file)
{
	std::scoped_lock<std::mutex> lock(m_SoundQueueLock);

	m_SoundLoaderQueue.emplace_back(file, new std::promise<std::shared_ptr<Sound>>());
	return m_SoundLoaderQueue.back().second->get_future();
}

std::future<std::shared_ptr<Music>> ResourceManager::LoadMusicAsync(const std::string& file)
{
	std::scoped_lock<std::mutex> lock(m_MusicQueueLock);

	m_MusicLoaderQueue.emplace_back(file, new std::promise<std::shared_ptr<Music>>());
	return m_MusicLoaderQueue.back().second->get_future();
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

		std::pair<std::string, std::shared_ptr<std::promise<std::shared_ptr<Sound>>>> entry;
		{
			std::scoped_lock<std::mutex> lock(m_SoundQueueLock);
			entry = queue.front();
			queue.pop_front();
		}
		entry.second->set_value(LoadSound(entry.first));
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

		std::pair<std::string, std::shared_ptr<std::promise<std::shared_ptr<Music>>>> entry;
		{
			std::scoped_lock<std::mutex> lock(m_MusicQueueLock);
			entry = queue.front();
			queue.pop_front();
		}
		entry.second->set_value(LoadMusic(entry.first));
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

		std::pair<std::string, std::shared_ptr<std::promise<std::shared_ptr<Surface2D>>>> entry;
		{
			std::scoped_lock<std::mutex> lock(m_SurfaceQueueLock);
			entry = queue.front();
			queue.pop_front();
		}
		entry.second->set_value(LoadSurface(entry.first));
	}
}

#pragma endregion

void ResourceManager::LoadFilePaths()
{

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


