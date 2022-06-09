#pragma once

#include "UtilityFiles/Singleton.h"
#include "UtilityFiles/Dictionary.h"
#include <string_view>

#define ENGINE Engine::GetInstance()

struct SDL_Window;

class Engine final : public Singleton<Engine>
{

	friend class Singleton<Engine>;

public:

	void Initialize();
	//void LoadGame() const;
	void Cleanup();
	void Run();

	float GetDeltaTime() const { return m_DeltaTime; }

	void SetResolution(int width, int height);

	void SetWindowDimentions(int width, int height);

	void GetResolution(int& width, int& height);

	void GetWindowDimensions(int& width, int& height);

	SDL_Window* GetWindow() const { return m_Window; }

	void InitializeSettings();

	void SaveSettings() const;

	const Dictionary& GetSettings() const { return m_EngineSettings; }

	void PauseGame(bool isPaused) { m_Paused = isPaused; }

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

	// Window data members
	SDL_Window* m_Window{};
	int m_ResolutionWidth = 640;
	int m_ResolutionHeight = 480;
	
	Dictionary m_EngineSettings;
};


namespace EngineSettings
{
	inline std::string_view engineConfig			{ "engineconfig.ini" };

	inline std::string_view gameResolutionX			{ "GameResolutionWidth" };
	inline std::string_view gameResolutionY			{ "GameResolutionHeight" };
	inline std::string_view editorResolutionX		{ "EditorResolutionWidth" };
	inline std::string_view editorResolutionY		{ "EditorResolutionHeight" };
	inline std::string_view gameWindowSizeX			{ "GameWindowSizeWidth" };
	inline std::string_view gameWindowSizeY			{ "GameWindowSizeHeight" };
	inline std::string_view editorWindowSizeX		{ "EditorWindowSizeWidth" };
	inline std::string_view editorWindowSizeY		{ "EditorWindowSizeHeight" };
	inline std::string_view gameWindowMaximized		{ "GameWindowMaximized" };
	inline std::string_view editorWindowMaximized	{ "EditorWindowMaximized" };
	inline std::string_view gameFullscreen			{ "GameFullscreen" };
	inline std::string_view editorFullscreen		{ "EditorFullscreen" };
	inline std::string_view rendererLayers			{ "RendererLayers" };
	inline std::string_view resourcePath			{ "ResourcesPath" };
	inline std::string_view editorKeepAspectRatio	{ "KeepAspectRatioEditor" };
	inline std::string_view gameTitle				{ "GameTitle" };
	inline std::string_view gameStartScene			{ "StartScene" };
}