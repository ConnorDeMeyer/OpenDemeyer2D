#pragma once
#include <iostream>
#include <functional>
#include <unordered_map>

enum class EngineSettings : size_t
{
	GameResolution,
	GameWindowResolution,
	EditorResolution,
	EditorWindowSize,
	GameWindowMaximized,
	EditorWindowMaximized,
	GameFullscreen,
	EditorFullscreen,
	KeepAspectRatioEditor,
	RendererLayers,
	ResourcesPath,

	// Insert engine settings here
	// Do not give these settings custom values as there are used as indices in an array


	ENGINE_SETTINGS_SIZE
};

constexpr const char* EngineSettingsStrings[]
{
	"GameResolution",
	"GameWindowResolution",
	"EditorResolution",
	"EditorWindowSize",
	"GameWindowMaximized",
	"EditorWindowMaximized",
	"GameFullscreen",
	"EditorFullscreen",
	"KeepAspectRatioEditor",
	"RendererLayers",
	"ResourcesPath"
};

static_assert(sizeof(EngineSettingsStrings) / sizeof(const char*) == size_t(EngineSettings::ENGINE_SETTINGS_SIZE));

class EngineSettingBase
{
public:
	EngineSettingBase(const std::string& settingName)
		: m_SettingsName{ settingName }
	{}

	virtual ~EngineSettingBase() = default;

	EngineSettingBase(const EngineSettingBase&) = delete;
	EngineSettingBase(EngineSettingBase&&) = delete;
	EngineSettingBase& operator=(const EngineSettingBase&) = delete;
	EngineSettingBase& operator=(EngineSettingBase&&) = delete;

	//virtual void DrawImgui() = 0;
	
	virtual void OnChange() = 0;

	virtual void OutPut(std::ostream& os)
	{
		os << m_SettingsName << ' ';
		OutputValue(os);
		os << '\n';
	}

	virtual void Input(std::istream& is)
	{
		InputValue(is);
	}

	const std::string& GetName() const { return m_SettingsName; }

protected:

	virtual void OutputValue(std::ostream& os) = 0;
	virtual void InputValue(std::istream& is) = 0;

	std::string m_SettingsName;

};

template <typename T>
class EngineSetting final : public EngineSettingBase
{
public:
	EngineSetting(const std::string& settingName, const T& defaultValue)
		: EngineSettingBase{settingName}
		, m_Value{defaultValue}
	{}

	virtual ~EngineSetting() = default;

private:

	void OutputValue(std::ostream& os) override
	{
		os << m_Value;
	}

	void InputValue(std::istream& is) override
	{
		is >> m_Value;
	}

	void OnChange() override
	{
		m_OnChangeFunction(m_Value);
	}

public:

	inline void SetOnChange(const std::function<void(T)>& function)
	{
		m_OnChangeFunction = function;
	}

private:
	T m_Value{};
	std::function<void(T)> m_OnChangeFunction{};
};

typedef std::unordered_map<std::string, std::unique_ptr<EngineSettingBase>> EngineSettingsMap;

void GenerateDefaultEngineSettings(EngineSettingsMap& map);
void ReadEngineSettings(std::istream& is, EngineSettingsMap& map);
void OutputEngineSettings(std::ostream& os, const EngineSettingsMap& map);