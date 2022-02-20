#pragma once

#include "Singleton.h"
#define ENGINE Engine::GetInstance()

struct SDL_Window;

class Engine final : public Singleton<Engine>
{

	friend class Singleton<Engine>;

public:

	void Initialize();
	void LoadGame() const;
	void Cleanup();
	void Run();

	void Quit() { m_Quit = true; }

	float GetDeltaTime() const { return m_DeltaTime; }

	void SetResolution(int width, int height);

	void SetWindowDimentions(int width, int height);

	void GetResolution(int& width, int& height);

	void GetWindowDimensions(int& width, int& height);

	SDL_Window* GetWindow() const { return m_Window; }

private:

	// Time and frame rate data members
	float m_MinimumFps = 1.f / 30.f;
	float m_TargetFps = 1.f / 60.f;
	float m_DeltaTime = 0.0f;

	bool m_Quit{};

	// Window data members
	SDL_Window* m_Window{};
	int m_ResolutionWidth = 640;
	int m_ResolutionHeight = 480;

};

