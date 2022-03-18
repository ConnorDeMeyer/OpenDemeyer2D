#include "OD2pch.h"
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
	m_EngineSettings.GetData(OD_EDITOR_RESOLUTION_WIDTH, m_ResolutionWidth);
	m_EngineSettings.GetData(OD_EDITOR_RESOLUTION_HEIGHT, m_ResolutionHeight);
	int w, h;
	m_EngineSettings.GetData(OD_EDITOR_WINDOW_SIZE_WIDTH, w);
	m_EngineSettings.GetData(OD_EDITOR_WINDOW_SIZE_HEIGHT, h);


	m_Window = SDL_CreateWindow(
		"Programming 4 assignment",
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
	bool maximizeWindow{};
	m_EngineSettings.GetData(OD_EDITOR_WINDOW_MAXIMIZED, maximizeWindow);
	if (maximizeWindow) {
		SDL_MaximizeWindow(m_Window);
	}

	RENDER.Init(m_Window);
}

//void Engine::LoadGame() const
//{
	//auto& scene = SceneManager::GetInstance().CreateScene("DemoScene");
	//
	//GameObject* go = new GameObject();
	//go->AddComponent<TrashTheCache>();
	//scene.Add(go);
	//
	//go = new GameObject();
	//go->AddComponent<RenderComponent>();
	//auto texture = go->AddComponent<TextureComponent>();
	//texture->SetTexture("34451b7e4347f9c02928f5360183a9e1ca98098c690d0f4209d81d9d3e7946a3.png");
	//scene.Add(go);
	//
	//GameObject* goc = new GameObject();
	//goc->AddComponent<RenderComponent>();
	//texture = goc->AddComponent<TextureComponent>();
	//texture->SetTexture("crosshair.png");
	//goc->SetParent(go);
//}

void Engine::Cleanup()
{
	RENDER.Destroy();

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
	m_EngineSettings.Insert("GameResolutionWidth", integer);
	m_EngineSettings.Insert("EditorResolutionWidth", integer);
	m_EngineSettings.Insert("GameWindowSizeWidth", integer);
	m_EngineSettings.Insert("EditorWindowSizeWidth", integer);

	integer = 1080;
	m_EngineSettings.Insert("GameResolutionHeight", integer);
	m_EngineSettings.Insert("EditorResolutionHeight", integer);
	m_EngineSettings.Insert("GameWindowSizeHeight", integer);
	m_EngineSettings.Insert("EditorWindowSizeHeight", integer);

	integer = 3;
	m_EngineSettings.Insert("RendererLayers", integer);

	bool boolean{ true };
	m_EngineSettings.Insert("GameWindowMaximized", boolean);
	m_EngineSettings.Insert("EditorWindowMaximized", boolean);
	m_EngineSettings.Insert("KeepAspectRatioEditor", boolean);

	boolean = false;
	m_EngineSettings.Insert("GameFullscreen", boolean);
	m_EngineSettings.Insert("EditorFullscreen", boolean);

	m_EngineSettings.Insert("ResourcesPath", std::string("Data/"));

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
	auto itBuffer = m_SettingsInputBuffer.begin();
	while (it != m_EngineSettings.end() && itBuffer != m_SettingsInputBuffer.end())
	{
		ImGui::InputText(it->first.c_str(), itBuffer->second, INPUT_TEXT_BUFFER_SIZE);

		++it;
		++itBuffer;
	}

	if (ImGui::Button("Save settings"))
	{
		SaveSettings();
	}

	ImGui::End();
}
