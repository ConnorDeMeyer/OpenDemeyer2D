#include "OD2pch.h"
#include "OpenDemeyer2D.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Scene.h"

#include "CustomComponents/TrashTheCache.h"
#include "Components/RenderComponent.h"
#include "Components/TextComponent.h"
#include "Components/TextureComponent.h"

#include <chrono>
#include <thread>

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
	PrintSDLVersion();

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	// Use OpenGL 3.11
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	m_ResolutionWidth = DM.w;
	m_ResolutionHeight = DM.h;

	m_Window = SDL_CreateWindow(
		"Programming 4 assignment",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1280,
		720,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	if (m_Window == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}
	
	SDL_MaximizeWindow(m_Window);

	RENDER.Init(m_Window);
}

void Engine::LoadGame() const
{
	auto& scene = SceneManager::GetInstance().CreateScene("DemoScene");

	GameObject* go = new GameObject();
	go->AddComponent<TrashTheCache>();
	scene.Add(go);
	
	go = new GameObject();
	go->AddComponent<RenderComponent>();
	auto texture = go->AddComponent<TextureComponent>();
	texture->SetTexture("34451b7e4347f9c02928f5360183a9e1ca98098c690d0f4209d81d9d3e7946a3.png");
	scene.Add(go);
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
	//SDL_RenderSetLogicalSize(RENDER.GetSDLRenderer(), width, height);
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
