#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <SDL.h>

#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <cassert>

#include <gl/glew.h>

#include <SDL_mixer.h>

#include "OpenDemeyer2D.h"
#include "InputManager.h"
#include "Singletons/SceneManager.h"
#include "RenderManager.h"
#include "Singletons/ResourceManager.h"
#include "Singletons/GUIManager.h"

#include "imgui.h"


using namespace std::chrono;

void PrintSDLVersion()
{
	SDL_version compiled{};
	SDL_version linked{};

	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	printf("We compiled against SDL version %d.%d.%d ...\n",
		compiled.major, compiled.minor, compiled.patch);
	printf("We are linking against SDL version %d.%d.%d.\n",
		linked.major, linked.minor, linked.patch);
}


void Engine::Initialize()
{
	InitializeSettings();

#if defined(DEBUG) || defined(_DEBUG)
	m_EngineSettings.GetData(EngineSettings::editorResolutionX.data(), m_ResolutionWidth);
	m_EngineSettings.GetData(EngineSettings::editorResolutionY.data(), m_ResolutionHeight);
	int w, h;
	m_EngineSettings.GetData(EngineSettings::editorWindowSizeX.data(), w);
	m_EngineSettings.GetData(EngineSettings::editorWindowSizeY.data(), h);
	bool maximizeWindow{};
	m_EngineSettings.GetData(EngineSettings::editorWindowMaximized.data(), maximizeWindow);
#else
	m_EngineSettings.GetData(EngineSettings::gameResolutionX.data(), m_ResolutionWidth);
	m_EngineSettings.GetData(EngineSettings::gameResolutionY.data(), m_ResolutionHeight);
	int w, h;
	m_EngineSettings.GetData(EngineSettings::gameWindowSizeX.data(), w);
	m_EngineSettings.GetData(EngineSettings::gameWindowSizeY.data(), h);
	bool maximizeWindow{};
	m_EngineSettings.GetData(EngineSettings::gameWindowMaximized.data(), maximizeWindow);
#endif

	/**
	* SDL
	*/
	PrintSDLVersion();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	// Use OpenGL 3.0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	/**
	* SDL_MIXER
	*/
	
	// load support for the OGG and MOD sample/music formats
	int flags = MIX_INIT_OGG | MIX_INIT_MOD;
	int initialized = Mix_Init(flags);
	if ((initialized & flags) != flags) {
		printf("Mix_Init: Failed to init required ogg and mod support!\n");
		printf("Mix_Init: %s\n", Mix_GetError());
		// handle error
	}

	// open 44.1KHz, signed 16bit, system byte order,
	//      stereo audio, using 1024 byte chunks
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		throw std::runtime_error(Mix_GetError());
	}

	/**
	* WINDOW
	*/

	std::string windowTitle;
	m_EngineSettings.GetData(EngineSettings::gameTitle.data(), windowTitle);

	m_Window = SDL_CreateWindow(
		windowTitle.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w,
		h,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	if (m_Window == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	// Maximize window if it the setting is on
	if (maximizeWindow) {
		SDL_MaximizeWindow(m_Window);
	}

	RENDER.Init(m_Window);

	// tell the resource manager where he can find the game data
	std::string dataPath;
	m_EngineSettings.GetData(EngineSettings::resourcePath.data(), dataPath);
	RESOURCES.Init(dataPath);

#ifdef _DEBUG
	GUI.Init(m_Window);
#endif
}

void Engine::Cleanup()
{
#ifdef _DEBUG
	GUI.Destroy();
#endif

	RENDER.Destroy();
	SCENES.Destroy();
	RESOURCES.Destroy();

	Mix_CloseAudio();

	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SDL_Quit();
}

void Engine::Run()
{
	Initialize();

	std::string startScenePath;
	m_EngineSettings.GetData(EngineSettings::gameStartScene.data(), startScenePath);
	auto startScene = RESOURCES.LoadScene(startScenePath);

#ifdef _DEBUG
	SCENES.SetActiveScene(startScene);
#else
	SCENES.PlayScene(startScene);
#endif


	{
		auto& renderer = RENDER;
		auto& sceneManager = SCENES;
		auto& input = INPUT;
		
		auto lastTime = high_resolution_clock::now();
		while (!m_Quit)
		{

			const auto currentTime = high_resolution_clock::now();
			float deltaTime = duration<float>(currentTime - lastTime).count();
			m_DeltaTime = std::min(deltaTime, m_MinimumFps);
			m_TimeLag += m_DeltaTime;

			input.ProcessInput();

			sceneManager.PreUpdate();

			if (!m_Paused)
			{
				for (; m_TimeLag >= 0.f; m_TimeLag -= m_PhysicsTimeStep)
					sceneManager.PhysicsStep(m_PhysicsTimeStep, m_PhysicsVelocityIter, m_PhysicsPositionIterations);
				sceneManager.Update(m_DeltaTime);
			}

			sceneManager.AfterUpdate();

			renderer.Render();


			const auto waitTime = currentTime + milliseconds(long long(m_TargetFps * 1000.f)) - high_resolution_clock::now();

			std::this_thread::sleep_for(waitTime);

			lastTime = currentTime;
		}
	}

	Cleanup();
}

void Engine::SetResolution(int width, int height)
{
	m_ResolutionHeight = height;
	m_ResolutionWidth = width;
}

void Engine::SetWindowDimentions(int width, int height)
{
	SDL_SetWindowSize(m_Window, width, height);
}

void Engine::GetResolution(int& w, int& h)
{
	SDL_GL_GetDrawableSize(m_Window, &w, &h);
}

void Engine::GetWindowDimensions(int& width, int& height)
{
	SDL_GetWindowSize(m_Window, &width, &height);
}

void Engine::InitializeSettings()
{
	// Load default values
	int integer{ 1920 };
	m_EngineSettings.Insert(EngineSettings::gameResolutionX.data(), integer);
	m_EngineSettings.Insert(EngineSettings::editorResolutionX.data(), integer);
	m_EngineSettings.Insert(EngineSettings::gameWindowSizeX.data(), integer);
	m_EngineSettings.Insert(EngineSettings::editorWindowSizeX.data(), integer);

	integer = 1080;
	m_EngineSettings.Insert(EngineSettings::gameResolutionY.data(), integer);
	m_EngineSettings.Insert(EngineSettings::editorResolutionY.data(), integer);
	m_EngineSettings.Insert(EngineSettings::gameWindowSizeY.data(), integer);
	m_EngineSettings.Insert(EngineSettings::editorWindowSizeY.data(), integer);

	integer = 3;
	m_EngineSettings.Insert(EngineSettings::rendererLayers.data(), integer);

	bool boolean{ true };
	m_EngineSettings.Insert(EngineSettings::gameWindowMaximized.data(), boolean);
	m_EngineSettings.Insert(EngineSettings::editorWindowMaximized.data(), boolean);
	m_EngineSettings.Insert(EngineSettings::editorKeepAspectRatio.data(), boolean);

	boolean = false;
	m_EngineSettings.Insert(EngineSettings::gameFullscreen.data(), boolean);
	m_EngineSettings.Insert(EngineSettings::editorFullscreen.data(), boolean);

	m_EngineSettings.Insert(EngineSettings::resourcePath.data(), std::string("Data/"));
	m_EngineSettings.Insert(EngineSettings::gameStartScene.data(), std::string("/"));
	m_EngineSettings.Insert(EngineSettings::gameTitle.data(), std::string("OpenDemeyer2D"));

	// Load the engineconfig.ini file
	auto fstream = std::ifstream(EngineSettings::engineConfig.data());

	if (!fstream.is_open()) return;

	std::string line;
	while (fstream)
	{
		std::getline(fstream, line);
		auto sstream = std::stringstream(line);
	
		{
			m_EngineSettings.StreamChange(sstream);
		}
	}
	
}

void Engine::SaveSettings() const
{
	auto os = std::ofstream(EngineSettings::engineConfig.data());

	auto it = m_EngineSettings.begin();
	while (it != m_EngineSettings.end())
	{
		os << it->first << ' ';
		it->second->OStreamValue(os);
		os << '\n';

		++it;
	}
}

void Engine::SaveGame()
{
	std::string title;
	m_EngineSettings.GetData(EngineSettings::gameTitle.data(), title);

	auto& sceneManager = SceneManager::GetInstance();
	auto of = std::ofstream(title.c_str());
	sceneManager.Serialize(of);
}
