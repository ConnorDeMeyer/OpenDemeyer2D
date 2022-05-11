#include "FileDetailView.h"

#include "imgui.h"

#include "ResourceWrappers/Texture2D.h"
#include "ResourceWrappers/Sound.h"
#include "Singletons/ResourceManager.h"

#include "EngineIO/Reflection.h"

constexpr std::string_view texture2D{ type_name<Texture2D>() };
constexpr std::string_view sound{ type_name<Sound>() };
constexpr std::string_view music{ type_name<Music>() };

std::string_view GetFileTypeFromExtension(const std::string extention)
{
	std::string ext{ extention };
	std::transform(ext.begin(), ext.end(), ext.begin(),
		[](char c) { return char(std::tolower(c)); });

	static std::unordered_map<std::string, std::string_view> FileExtentionMap
	{
		{".voc"		, music},
		{".flac"	, music},
		{".off"		, music},
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
		return new MusicDetailView(path, RESOURCES.LoadMusic(path.string(), true));

	if (file == sound)
		return new SoundDetailView(path, RESOURCES.LoadSound(path.string(), true));

	if (file == texture2D)
		return new ImageDetailView(path, RESOURCES.LoadTexture(path.string(), true));

	return new EmptyFileDetailView(path);
}


void ImageDetailView::RenderDetails()
{
	if (!m_Texture)
		return;

	ImGui::Text("Path:   [%s]", m_Texture->GetFilePath().c_str());
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

void MusicDetailView::RenderDetails()
{
	if (!m_Music)
		return;

	ImGui::Text("Path: [%s]", m_Music->GetFilePath().c_str());

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

void SoundDetailView::RenderDetails()
{
	if (!m_Sound)
		return;

	ImGui::Text("Path: [%s]", m_Sound->GetFilePath().c_str());

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

