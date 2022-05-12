#pragma once

#include "UtilityFiles/Singleton.h"
#include "UtilityFiles/Dictionary.h"

#define ENGINE Engine::GetInstance()

struct SDL_Window;
class GameInstance;

class Engine final : public Singleton<Engine>
{

	friend class Singleton<Engine>;

public:

	void Initialize();
	//void LoadGame() const;
	void Cleanup();
	void Run(GameInstance* pGameInstance);

	float GetDeltaTime() const { return m_DeltaTime; }

	void SetResolution(int width, int height);

	void SetWindowDimentions(int width, int height);

	void GetResolution(int& width, int& height);

	void GetWindowDimensions(int& width, int& height);

	SDL_Window* GetWindow() const { return m_Window; }

	void InitializeSettings();

	void SaveSettings() const;

	const Dictionary& GetSettings() const { return m_EngineSettings; }

	void PlayGame();

	void PauseGame(bool isPaused) { m_Paused = isPaused; }

	void StopGame();

	void Quit() { m_Quit = true; }

	void SaveGame();

private:

	// Time and frame rate data members
	float m_MinimumFps = 1.f / 30.f;
	float m_TargetFps = 1.f / 60.f;
	float m_DeltaTime = 0.0f;

	float m_PhysicsTimeStep = 1.f / 60.f;
	int m_PhysicsVelocityIter = 10;
	int m_PhysicsPositionIterations = 8;
	float m_TimeLag{};

	bool m_Quit{};
	bool m_Paused{};
	bool m_Playing{};

	// Window data members
	SDL_Window* m_Window{};
	int m_ResolutionWidth = 640;
	int m_ResolutionHeight = 480;
	
	Dictionary m_EngineSettings;

	GameInstance* m_pGameinstance{};
};

#pragma region EngineSettings

#define OD_ENGINE_CONFIG_INI "engineconfig.ini"

#define OD_GAME_RESOLUTION_WIDTH "GameResolutionWidth"
#define OD_GAME_RESOLUTION_HEIGHT "GameResolutionHeight"
#define OD_EDITOR_RESOLUTION_WIDTH "EditorResolutionWidth"
#define OD_EDITOR_RESOLUTION_HEIGHT "EditorResolutionHeight"
#define OD_GAME_WINDOW_SIZE_WIDTH "GameWindowSizeWidth"
#define OD_GAME_WINDOW_SIZE_HEIGHT "GameWindowSizeHeight"
#define OD_EDITOR_WINDOW_SIZE_WIDTH "EditorWindowSizeWidth"
#define OD_EDITOR_WINDOW_SIZE_HEIGHT "EditorWindowSizeHeight"
#define OD_GAME_WINDOW_MAXIMIZED "GameWindowMaximized"
#define OD_EDITOR_WINDOW_MAXIMIZED "EditorWindowMaximized"
#define OD_GAME_FULLSCREEN "GameFullscreen"
#define OD_EDITOR_FULLSCREEN "EditorFullscreen"
#define OD_RENDERER_LAYERS "RendererLayers"
#define OD_RESOURCES_PATH "ResourcesPath"
#define OD_KEEP_ASPECT_RATIO_EDITOR "KeepAspectRatioEditor"
#define OD_GAME_TITLE "GameTitle"

#pragma endregion