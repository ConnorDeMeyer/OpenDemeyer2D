#include "OD2pch.h"
#include "OpenDemeyer2D.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Scene.h"

#include <chrono>
#include <thread>

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
	PrintSDLVersion();

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	m_Window = SDL_CreateWindow(
		"Programming 4 assignment",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_OPENGL
	);
	if (m_Window == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	RENDER.Init(m_Window);
}

void Engine::LoadGame() const
{
	SceneManager::GetInstance().CreateScene("Demo");
}

void Engine::Cleanup()
{
	RENDER.Destroy();

	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SDL_Quit();
}

void Engine::Run()
{
	Initialize();

	// tell the resource manager where he can find the game data
	ResourceManager::GetInstance().Init("../Data/");

	LoadGame();

	{
		auto& renderer = RENDER;
		auto& sceneManager = SceneManager::GetInstance();
		auto& input = InputManager::GetInstance();
		
		auto lastTime = high_resolution_clock::now();
		while (!m_Quit)
		{
			const auto currentTime = high_resolution_clock::now();
			float deltaTime = duration<float>(currentTime - lastTime).count();
			deltaTime = std::clamp(deltaTime, m_TargetFps, m_MinimumFps);

			input.ProcessInput();
			sceneManager.Update(deltaTime);
			renderer.Render();

			lastTime = currentTime;
		}
	}

	Cleanup();
}
