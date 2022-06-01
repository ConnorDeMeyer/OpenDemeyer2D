#include "SoundLoaderTest.h"
#include "Singletons/ResourceManager.h"
#include <imgui.h>
#include <chrono>

using namespace std::chrono_literals;

void SoundLoaderTest::RenderImGui()
{
	static char soundPathBuffer[256]{};

	ImGui::Text("Sounds");
	ImGui::InputText("File", soundPathBuffer, 256);
	if (ImGui::Button("Load Sound File"))
	{
		m_Sounds.emplace_back(RESOURCES.LoadSound(std::filesystem::path{ soundPathBuffer }));
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Sound File Async"))
	{
		m_Sounds.emplace_back(RESOURCES.LoadSoundAsync(std::filesystem::path{ soundPathBuffer }));
	}

	if (ImGui::CollapsingHeader("Sounds"))
	{
		for (auto& sound : m_Sounds)
		{
			if (sound)
			{
				ImGui::Text(sound->GetFilePath().string().c_str());
				if (ImGui::Button("Play"))
				{
					sound->Play();
				}
			}
			else
			{
				ImGui::TextDisabled(sound->GetFilePath().string().c_str());
			}
		}
	}

	ImGui::Separator();

	static char musicPathBuffer[256]{};

	ImGui::Text("Sounds");
	ImGui::InputText("File", musicPathBuffer, 256);
	if (ImGui::Button("Load Sound File"))
	{
		m_Music.emplace_back(RESOURCES.LoadMusic(std::filesystem::path{ musicPathBuffer }));
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Sound File Async"))
	{
		m_Music.emplace_back(RESOURCES.LoadMusicAsync(std::filesystem::path{ musicPathBuffer }));
	}

	if (ImGui::CollapsingHeader("Sounds"))
	{
		for (auto& music : m_Music)
		{
			if (music)
			{
				ImGui::Text(music->GetFilePath().string().c_str());
				if (ImGui::Button("Play"))
				{
					music->PlayMusic(1, 500);
				}
				if (ImGui::Button("Pause"))
				{
					music->PauseMusic();
				}
				if (ImGui::Button("Resume"))
				{
					music->ResumeMusic();
				}
				float volume{ music->GetMusicVolume() };
				if (ImGui::SliderFloat("Volume", &volume, 0.f, 1.f))
				{
					music->SetMusicVolume(volume);
				}
				if (ImGui::Button("Rewind"))
				{
					music->RewindMusic();
				}
			}
			else
			{
				ImGui::TextDisabled(music->GetFilePath().string().c_str());
			}
		}
	}
}
