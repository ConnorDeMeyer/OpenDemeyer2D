#include "pch.h"
#include "FileDetailView.h"

#include "imgui.h"

#include "ResourceWrappers/Texture2D.h"
#include "ResourceWrappers/Sound.h"
#include "ResourceWrappers/Prefab.h"
#include "Singletons/ResourceManager.h"
#include "EngineFiles/GameObject.h"
#include "EngineFiles/Scene.h"
#include "EngineFiles/ComponentBase.h"

#include "EngineIO/Reflection.h"

constexpr std::string_view texture2D{ type_name<Texture2D>() };
constexpr std::string_view sound{ type_name<Sound>() };
constexpr std::string_view music{ type_name<Music>() };
constexpr std::string_view prefab{ type_name<Prefab>() };
constexpr std::string_view scene{ "SceneFile" };

std::string_view GetFileTypeFromExtension(const std::string extention)
{
	std::string ext{ extention };
	std::transform(ext.begin(), ext.end(), ext.begin(),
		[](char c) { return char(std::tolower(c)); });

	static std::unordered_map<std::string, std::string_view> FileExtentionMap
	{
		{".gobj"	,prefab},

		{".scene"	,scene},

		{".voc"		, music},
		{".flac"	, music},
		{".ogg"		, music},
		{".mp3"		, music},
		{".midi"	, music},
		{".mid"		, music},
		{".mod"		, music},
		{".s3m"		, music},
		{".it"		, music},
		{".xm"		, music},

		{".wav"		, sound},
		{".wave"	, sound},

		{".avif"	, texture2D},
		{".bmp"		, texture2D},
		{".gif"		, texture2D},
		{".jpeg"	, texture2D},
		{".jpeg-xl" , texture2D},
		{".lbm"		, texture2D},
		{".pcx"		, texture2D},
		{".png"		, texture2D},
		{".pnm"		, texture2D},
		{".ppm"		, texture2D},
		{".phm"		, texture2D},
		{".pbm"		, texture2D},
		{".qoi"		, texture2D},
		{".tga"		, texture2D},
		{".tiff"	, texture2D},
		{".webp"	, texture2D},
		{".xcf"		, texture2D},
		{".svg"		, texture2D}
	};

	return FileExtentionMap[ext];
}

bool FileDetailView::RenderImGuiFileName()
{
	bool returnValue{};

	if (ImGui::Selectable(GetFileName().c_str()))
	{
		returnValue = true;
	}
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		std::string fileClass{ GetFileClass() };
		ImGui::SetDragDropPayload(fileClass.c_str(), GetPackage(), GetPackageSize());

		ImGui::Text(fileClass.c_str());

		ImGui::EndDragDropSource();
	}
	return returnValue;
}

FileDetailView* FileDetailView::FileDetailFactory(const std::filesystem::path& path)
{
	std::string extension = path.extension().string();
	std::string_view file = GetFileTypeFromExtension(extension);

	if (file == music)
		return new MusicDetailView(path, RESOURCES.LoadMusic(path, true));

	if (file == sound)
		return new SoundDetailView(path, RESOURCES.LoadSound(path, true));

	if (file == texture2D)
		return new ImageDetailView(path, RESOURCES.LoadTexture(path, true));

	if (file == prefab)
		return new PrefabDetailView(path, RESOURCES.LoadPrefab(path, true));

	if (file == scene)
		return new SceneDetailView(path);

	return new EmptyFileDetailView(path);
}

// ***********************************************
// IMAGE DETAIL VIEW
// ***********************************************

void ImageDetailView::RenderDetails()
{
	if (!m_Texture)
		return;

	ImGui::TextWrapped("Path:   [%s]", m_Texture->GetFilePath().c_str());
	ImGui::Text("Width:  [%d]", m_Texture->GetWidth());
	ImGui::Text("Height: [%d]", m_Texture->GetHeight());

	float width = ImGui::GetWindowWidth() - 20.f;

#pragma warning(disable : 4312)
	ImGui::Image((ImTextureID)(m_Texture->GetId()), { width,width / m_Texture->GetWidth() * m_Texture->GetHeight() });
#pragma warning(default : 4312)
}

constexpr std::string_view ImageDetailView::GetFileClass()
{
	return type_name<Texture2D>();
}

void* ImageDetailView::GetPackage()
{
	return &m_Texture;
}

size_t ImageDetailView::GetPackageSize()
{
	return sizeof(std::shared_ptr<Texture2D>);
}

// ***********************************************
// MUSIC DETAIL VIEW
// ***********************************************

void MusicDetailView::RenderDetails()
{
	if (!m_Music)
		return;

	ImGui::TextWrapped("Path: [%s]", m_Music->GetFilePath().c_str());

	if (ImGui::Button("Play"))
	{
		m_Music->PlayMusic(1, 500);
	}
	if (ImGui::Button("Pause"))
	{
		m_Music->PauseMusic();
	}
	if (ImGui::Button("Resume"))
	{
		m_Music->ResumeMusic();
	}
	float volume{ m_Music->GetMusicVolume() };
	if (ImGui::SliderFloat("Volume", &volume, 0.f, 1.f))
	{
		m_Music->SetMusicVolume(volume);
	}
	if (ImGui::Button("Rewind"))
	{
		m_Music->RewindMusic();
	}
}

constexpr std::string_view MusicDetailView::GetFileClass()
{
	return type_name<Music>();
}

void* MusicDetailView::GetPackage()
{
	return &m_Music;
}

size_t MusicDetailView::GetPackageSize()
{
	return sizeof(std::shared_ptr<Music>);
}

// ***********************************************
// SOUND DETAIL VIEW
// ***********************************************

void SoundDetailView::RenderDetails()
{
	if (!m_Sound)
		return;

	ImGui::TextWrapped("Path: [%s]", m_Sound->GetFilePath().c_str());

	if (ImGui::Button("Play"))
	{
		m_Sound->Play(0);
	}
	if (ImGui::Button("Pause"))
	{
		m_Sound->Pause();
	}
	if (ImGui::Button("Resume"))
	{
		m_Sound->Resume();
	}
	float volume{ m_Sound->GetVolume() };
	if (ImGui::SliderFloat("Volume", &volume, 0.f, 1.f))
	{
		m_Sound->SetVolume(volume);
	}
}

constexpr std::string_view SoundDetailView::GetFileClass()
{
	return type_name<Sound>();
}

void* SoundDetailView::GetPackage()
{
	return &m_Sound;
}

size_t SoundDetailView::GetPackageSize()
{
	return sizeof(std::shared_ptr<Sound>);
}

// ***********************************************
// PREFAB DETAIL VIEW
// ***********************************************

void RenderGameObject(GameObject* go)
{
	ImGui::PushID(go);


	if (ImGui::TreeNodeEx(go->GetDisplayName().c_str(),
		ImGuiTreeNodeFlags_Leaf * go->GetChildren().empty() | ImGuiTreeNodeFlags_OpenOnArrow))
	{
		// Component display
		if (ImGui::TreeNode("Components"))
		{
			for (auto& comp : go->GetComponents())
			{
				std::string name{ comp.second->GetComponentName() };
				ImGui::Text(name.c_str());
			}
			ImGui::TreePop();
		}

		// children
		for (auto child : go->GetChildren())
			RenderGameObject(child);

		ImGui::TreePop();
	}

	ImGui::PopID();
}

void PrefabDetailView::RenderDetails()
{
	if (m_Prefab)
	{
		RenderGameObject(m_Prefab->GetGameObject());
	}
}

constexpr std::string_view PrefabDetailView::GetFileClass()
{
	return prefab;
}

void* PrefabDetailView::GetPackage()
{
	return &m_Prefab;
}

size_t PrefabDetailView::GetPackageSize()
{
	return sizeof(GameObject*);
}

// ***********************************************
// SCENE DETAIL VIEW
// ***********************************************

SceneDetailView::SceneDetailView(const std::filesystem::path& path)
	: FileDetailView(path)
{
	//m_Scene->PreUpdate(false);
	//m_Scene->AfterUpdate();
}

void SceneDetailView::RenderDetails()
{
	if (ImGui::Button("Open Scene"))
	{
		try
		{
			SCENES.SetActiveScene(RESOURCES.LoadScene(m_Path));
		}
		catch (std::exception&)
		{
			
		}
	}
}

constexpr std::string_view SceneDetailView::GetFileClass()
{
	return scene;
}

void* SceneDetailView::GetPackage()
{
	return &m_Path;
}

size_t SceneDetailView::GetPackageSize()
{
	return sizeof(std::filesystem::path*);
}
