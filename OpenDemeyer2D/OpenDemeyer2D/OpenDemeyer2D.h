#pragma once

#include "Singleton.h"
#include "Dictionary.h"

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

	void Quit() { m_Quit = true; }

	float GetDeltaTime() const { return m_DeltaTime; }

	void SetResolution(int width, int height);

	void SetWindowDimentions(int width, int height);

	void GetResolution(int& width, int& height);

	void GetWindowDimensions(int& width, int& height);

	SDL_Window* GetWindow() const { return m_Window; }

	/** Render ImGui stats of the engine*/
	void RenderStats();

	void InitializeSettings();

	void SaveSettings() const;

	const Dictionary& GetSettings() const { return m_EngineSettings; }

	/** Render the settings in an ImGui window*/
	void RenderSettings();

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
	
	Dictionary m_EngineSettings;

	// ImGui data members
	const size_t INPUT_TEXT_BUFFER_SIZE = 128;
	std::unordered_map<std::string,char*> m_SettingsInputBuffer;

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

#pragma endregion