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

private:

	float m_MinimumFps = 1.f / 30.f;
	float m_TargetFps = 1.f / 60.f;

	bool m_Quit{};

	SDL_Window* m_Window{};

};

