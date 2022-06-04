#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <SDL.h>

#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <cassert>

#include <gl/glew.h>
#include <gl/wglew.h>

#include <SDL_mixer.h>

#include "OpenDemeyer2D.h"
#include "InputManager.h"
#include "Singletons/SceneManager.h"
#include "RenderManager.h"
#include "Singletons/ResourceManager.h"
#include "Singletons/GUIManager.h"
#include "EngineFiles/GameObject.h"
#include "EngineFiles/Scene.h"
#include "ResourceWrappers/Sound.h"
#include "EngineIO/CustomSerializers.h"

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
	m_EngineSettings.GetData(OD_EDITOR_RESOLUTION_WIDTH, m_ResolutionWidth);
	m_EngineSettings.GetData(OD_EDITOR_RESOLUTION_HEIGHT, m_ResolutionHeight);
	int w, h;
	m_EngineSettings.GetData(OD_EDITOR_WINDOW_SIZE_WIDTH, w);
	m_EngineSettings.GetData(OD_EDITOR_WINDOW_SIZE_HEIGHT, h);
	bool maximizeWindow{};
	m_EngineSettings.GetData(OD_EDITOR_WINDOW_MAXIMIZED, maximizeWindow);
#else
	m_EngineSettings.GetData(OD_GAME_RESOLUTION_HEIGHT, m_ResolutionWidth);
	m_EngineSettings.GetData(OD_GAME_RESOLUTION_HEIGHT, m_ResolutionHeight);
	int w, h;
	m_EngineSettings.GetData(OD_GAME_WINDOW_SIZE_WIDTH, w);
	m_EngineSettings.GetData(OD_GAME_WINDOW_SIZE_HEIGHT, h);
	bool maximizeWindow{};
	m_EngineSettings.GetData(OD_GAME_WINDOW_MAXIMIZED, maximizeWindow);
#endif

	/**
	* SDL
	*/
	PrintSDLVersion();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	// Use OpenGL 3.11
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

	/**
	* SDL_MIXER
	*/

	// open 44.1KHz, signed 16bit, system byte order,
	//      stereo audio, using 1024 byte chunks
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		throw std::runtime_error(Mix_GetError());
	}

	/**
	* WINDOW
	*/

	std::string windowTitle;
	m_EngineSettings.GetData(OD_GAME_TITLE, windowTitle);

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
	m_EngineSettings.GetData(OD_RESOURCES_PATH, dataPath);
	RESOURCES.Init(dataPath);

	GUI.Init(m_Window);
}

void Engine::Cleanup()
{
	GUI.Destroy();
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
	m_EngineSettings.GetData(OD_GAME_START_SCENE, startScenePath);

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
	m_EngineSettings.Insert(OD_GAME_RESOLUTION_WIDTH, integer);
	m_EngineSettings.Insert(OD_EDITOR_RESOLUTION_WIDTH, integer);
	m_EngineSettings.Insert(OD_GAME_WINDOW_SIZE_WIDTH, integer);
	m_EngineSettings.Insert(OD_EDITOR_WINDOW_SIZE_WIDTH, integer);

	integer = 1080;
	m_EngineSettings.Insert(OD_GAME_RESOLUTION_HEIGHT, integer);
	m_EngineSettings.Insert(OD_EDITOR_RESOLUTION_HEIGHT, integer);
	m_EngineSettings.Insert(OD_GAME_WINDOW_SIZE_HEIGHT, integer);
	m_EngineSettings.Insert(OD_EDITOR_WINDOW_SIZE_HEIGHT, integer);

	integer = 3;
	m_EngineSettings.Insert(OD_RENDERER_LAYERS, integer);

	bool boolean{ true };
	m_EngineSettings.Insert(OD_GAME_WINDOW_MAXIMIZED, boolean);
	m_EngineSettings.Insert(OD_EDITOR_WINDOW_MAXIMIZED, boolean);
	m_EngineSettings.Insert(OD_KEEP_ASPECT_RATIO_EDITOR, boolean);

	boolean = false;
	m_EngineSettings.Insert(OD_GAME_FULLSCREEN, boolean);
	m_EngineSettings.Insert(OD_EDITOR_FULLSCREEN, boolean);

	m_EngineSettings.Insert(OD_RESOURCES_PATH, std::string("Data/"));
	m_EngineSettings.Insert(OD_GAME_START_SCENE, std::string("/"));
	m_EngineSettings.Insert(OD_GAME_TITLE, std::string("OpenDemeyer2D"));

	// Load the engineconfig.ini file
	auto fstream = std::ifstream(OD_ENGINE_CONFIG_INI);

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
	auto os = std::ofstream(OD_ENGINE_CONFIG_INI);

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
	m_EngineSettings.GetData(OD_GAME_TITLE, title);

	auto& sceneManager = SceneManager::GetInstance();
	auto of = std::ofstream(title.c_str());
	sceneManager.Serialize(of);
}
