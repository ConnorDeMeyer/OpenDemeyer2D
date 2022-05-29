#include "EngineSettings.h"

#include <glm/glm.hpp>
#include "ImGuiExt/ImGuiExt.hpp"

#include "Singletons/GUIManager.h"
#include "Singletons/RenderManager.h"
#include "Singletons/OpenDemeyer2D.h"
#include "Singletons/SceneManager.h"

#include "CustomSerializers.h"

void GenerateDefaultEngineSettings(EngineSettingsMap& map)
{
	auto GameResolution = new EngineSetting("GameResolution", glm::ivec2{1920,1080});
	GameResolution->SetOnChange([](const glm::ivec2& val)
		{
			RENDER.SetRenderResolution(val.x, val.y);
		});
	
	auto GameWindowResolution = new EngineSetting("GameWindowResolution", glm::ivec2{ 1920,1080 });
	GameWindowResolution->SetOnChange([](const glm::ivec2 val)
		{
#ifndef _DEBUG || DEBUG
			ENGINE.SetWindowDimentions(val.x, val.y);
#endif
		});

	auto EditorResolution = new EngineSetting("EditorResolution", glm::ivec2{1920,1080});
	EditorResolution->SetOnChange([](const glm::ivec2&)
		{

		});

	auto EditorWindowSize = new EngineSetting("EditorWindowSize", glm::ivec2{1920,1080});
	EditorWindowSize->SetOnChange([](const glm::ivec2& val)
		{
#ifdef _DEBUG || DEBUG
			ENGINE.SetWindowDimentions(val.x, val.y);
#endif
		});

	auto GameWindowMaximized = new EngineSetting("GameWindowMaximized", true);
	GameWindowMaximized->SetOnChange([](bool val)
		{
#ifndef _DEBUG || DEBUG
			if (val)
				ENGINE.MaximizeWindow();
#endif
		});

	auto EditorWindowMaximized = new EngineSetting("EditorWindowMaximized", true);
	EditorWindowMaximized->SetOnChange([](bool val)
		{
#ifdef _DEBUG || DEBUG
			if (val)
				ENGINE.MaximizeWindow();
#endif
		});

	auto GameFullscreen = new EngineSetting("GameFullscreen", false);
	GameFullscreen->SetOnChange([](bool val)
		{
#ifndef _DEBUG || DEBUG
			if (val)
				ENGINE.WindowFullScreenMode();
#endif
		});

	auto EditorFullscreen = new EngineSetting("EditorFullscreen", false);
	EditorFullscreen->SetOnChange([](bool val)
		{
#ifdef _DEBUG || DEBUG
			if (val)
				ENGINE.WindowFullScreenMode();
#endif
		});

	auto KeepAspectRatioEditor = new EngineSetting("KeepAspectRatioEditor", true);
	KeepAspectRatioEditor->SetOnChange([](bool val)
		{
			GUI.KeepAspectRatio(val);
		});

	auto RendererLayers = new EngineSetting("RendererLayers", 3);
	RendererLayers->SetOnChange([](int val)
		{
			RENDER.SetRenderLayers(val);
		});

	auto ResourcesPath = new EngineSetting("ResourcesPath", "Data/");



	map.reserve(size_t(EngineSettings::ENGINE_SETTINGS_SIZE));

	map.emplace(EngineSettingsStrings[size_t(EngineSettings::GameResolution)], GameResolution);
	map.emplace(EngineSettingsStrings[size_t(EngineSettings::GameWindowResolution)], GameWindowResolution);
	map.emplace(EngineSettingsStrings[size_t(EngineSettings::EditorResolution)], EditorResolution);
	map.emplace(EngineSettingsStrings[size_t(EngineSettings::EditorWindowSize)], EditorWindowSize);
	map.emplace(EngineSettingsStrings[size_t(EngineSettings::GameWindowMaximized)], GameWindowMaximized);
	map.emplace(EngineSettingsStrings[size_t(EngineSettings::EditorWindowMaximized)], EditorWindowMaximized);
	map.emplace(EngineSettingsStrings[size_t(EngineSettings::GameFullscreen)], GameFullscreen);
	map.emplace(EngineSettingsStrings[size_t(EngineSettings::EditorFullscreen)], EditorFullscreen);
	map.emplace(EngineSettingsStrings[size_t(EngineSettings::KeepAspectRatioEditor)], KeepAspectRatioEditor);
	map.emplace(EngineSettingsStrings[size_t(EngineSettings::RendererLayers)], RendererLayers);
	map.emplace(EngineSettingsStrings[size_t(EngineSettings::ResourcesPath)], ResourcesPath);
}

void ReadEngineSettings(std::istream& is, EngineSettingsMap& map)
{

}

void OutputEngineSettings(std::ostream& os, const EngineSettingsMap& map)
{

}
