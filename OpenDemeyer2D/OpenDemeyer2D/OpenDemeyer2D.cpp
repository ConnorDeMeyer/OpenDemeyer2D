﻿#include "OD2pch.h"
#include "OpenDemeyer2D.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "GameInstance.h"

#include "CustomComponents/TrashTheCache.h"
#include "Components/RenderComponent.h"
#include "Components/TextComponent.h"
#include "Components/TextureComponent.h"

#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <cassert>

#include <gl/glew.h>
#include <gl/wglew.h>


#include <steam_api.h>

#include "SteamAPI.h"

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

	PrintSDLVersion();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	// Use OpenGL 3.11
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);

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

	/** Initialize steam*/
	if (!SteamAPI_Init())
	{
		std::cout << "Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed)." << std::endl;
	}
	else
	{
		ACHIEVEMENTS.Init();
		std::cout << "Successfully initialized steam." << std::endl;
	}

	RENDER.Init(m_Window);
}

void Engine::Cleanup()
{
	RENDER.Destroy();

	// Close steam
	SteamAPI_Shutdown();

	// cleanup ImGui variables
	for (auto& buffer : m_SettingsInputBuffer)
		delete[] buffer.second;

	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SDL_Quit();
}

void Engine::Run(GameInstance* pGameInstance)
{
	Initialize();

	// tell the resource manager where he can find the game data
	std::string dataPath;
	m_EngineSettings.GetData(OD_RESOURCES_PATH, dataPath);
	ResourceManager::GetInstance().Init(dataPath);

	pGameInstance->LoadGame();
	m_pGameinstance = pGameInstance;

	{
		auto& renderer = RENDER;
		auto& sceneManager = SceneManager::GetInstance();
		auto& input = InputManager::GetInstance();
		
		auto lastTime = high_resolution_clock::now();
		while (!m_Quit)
		{
			SteamAPI_RunCallbacks();

			const auto currentTime = high_resolution_clock::now();
			float deltaTime = duration<float>(currentTime - lastTime).count();
			m_DeltaTime = std::min(deltaTime, m_MinimumFps);

			input.ProcessInput();
			sceneManager.Update(m_DeltaTime);
			renderer.Render();


			//std::this_thread::sleep_for(m_TargetFps - (high_resolution_clock::now() - currentTime))
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

void Engine::RenderStats()
{
	ImGui::Begin("Statistics");

	ImGui::Text("Application average %.3f ms/frame", 1000.f / ImGui::GetIO().Framerate);
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("Resolution: %.3i x %.1i", m_ResolutionWidth, m_ResolutionHeight);

	ImGui::End();
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

	// ImGui settings window setup
	auto it = m_EngineSettings.begin();
	while (it != m_EngineSettings.end())
	{
		std::string key = it->first;

		auto sstream = std::stringstream();

		it->second->OStreamValue(sstream);

		std::string value;
		sstream >> value;

		// check if the setting isn't already in there
		assert(m_SettingsInputBuffer.find(key) == m_SettingsInputBuffer.end());

		char* buffer = new char[INPUT_TEXT_BUFFER_SIZE] {};
		_memccpy(buffer, value.c_str(), '\0', value.size());
		m_SettingsInputBuffer.insert(std::make_pair(key, buffer));

		++it;
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

void Engine::RenderSettings()
{
	ImGui::Begin("Engine Settings");

	auto it = m_EngineSettings.begin();
	while (it != m_EngineSettings.end())
	{
		auto itBuffer = m_SettingsInputBuffer.find(it->first);
		if (itBuffer != m_SettingsInputBuffer.end())
		{
			ImGui::InputText(it->first.c_str(), itBuffer->second, INPUT_TEXT_BUFFER_SIZE);
		}
		++it;
	}

	if (ImGui::Button("Save settings"))
	{
		SaveSettings();
	}

	ImGui::End();
}