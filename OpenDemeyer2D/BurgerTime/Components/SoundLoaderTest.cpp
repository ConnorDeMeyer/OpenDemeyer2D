#include "SoundLoaderTest.h"
#include "Singletons/ResourceManager.h"
#include <imgui.h>
#include <chrono>

using namespace std::chrono_literals;

void SoundLoaderTest::RenderImGui()
{
	if (ImGui::Button("Load My Ashes") && !m_IsLoadingMyAshes)
	{
		m_IsLoadingMyAshes = true;
		m_FutureMyAshes = RESOURCES.LoadMusicAsync("Audio/Porcupine_Tree-My_Ashes.mp3");
	}

	if (m_IsLoadingMyAshes && m_FutureMyAshes.wait_for(0s) == std::future_status::ready)
	{
		m_IsLoadingMyAshes = false;
		m_MusicMyAshes = std::static_pointer_cast<Music>(m_FutureMyAshes.get());
	}

	if (m_MusicMyAshes)
	{
		if (ImGui::Button("Play My Ashes"))
		{
			m_MusicMyAshes->PlayMusic(1, 500);
		}
		if (ImGui::Button("Pause My Ashes"))
		{
			m_MusicMyAshes->PauseMusic();
		}
		if (ImGui::Button("Resume My Ashes"))
		{
			m_MusicMyAshes->ResumeMusic();
		}
		float volume{ m_MusicMyAshes->GetMusicVolume() };
		if (ImGui::SliderFloat("Volume My Ashes", &volume, 0.f, 1.f))
		{
			m_MusicMyAshes->SetMusicVolume(volume);
		}
		if (ImGui::Button("Rewind My Ashes"))
		{
			m_MusicMyAshes->RewindMusic();
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Load Valkerie") && !m_IsLoadingValkerie)
	{
		m_IsLoadingValkerie = true;
		m_FutureValkerie = RESOURCES.LoadMusicAsync("Audio/Varien-The_Valkyrie_Trilogy.mp3");
	}

	if (m_IsLoadingValkerie && m_FutureValkerie.wait_for(0s) == std::future_status::ready)
	{
		m_IsLoadingValkerie = false;
		m_MusicValkerie = std::static_pointer_cast<Music>(m_FutureValkerie.get());
	}

	if (m_MusicValkerie)
	{
		if (ImGui::Button("Play Valkerie"))
		{
			m_MusicValkerie->PlayMusic(1, 500);
		}
		if (ImGui::Button("Pause Valkerie"))
		{
			m_MusicValkerie->PauseMusic();
		}
		if (ImGui::Button("Resume Valkerie"))
		{
			m_MusicValkerie->ResumeMusic();
		}
		float volume{ m_MusicValkerie->GetMusicVolume() };
		if (ImGui::SliderFloat("Volume Valkerie", &volume, 0.f, 1.f))
		{
			m_MusicValkerie->SetMusicVolume(volume);
		}
		if (ImGui::Button("Rewind Valkerie"))
		{
			m_MusicValkerie->RewindMusic();
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Load Hop") && !m_IsLoadingHop)
	{
		m_IsLoadingHop = true;
		m_FutureHop = RESOURCES.LoadSoundAsync("Audio/sound-frogger-hop.wav");
	}

	if (m_IsLoadingHop && m_FutureHop.wait_for(0s) == std::future_status::ready)
	{
		m_IsLoadingHop = false;
		m_SoundHop = std::static_pointer_cast<Sound>(m_FutureHop.get());
	}

	if (m_SoundHop)
	{
		if (ImGui::Button("Play Hop")) 
		{
			m_SoundHop->Play(0);
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Load Plunk") && !m_IsLoadingPlunk)
	{
		m_IsLoadingPlunk = true;
		m_FuturePlunk = RESOURCES.LoadSoundAsync("Audio/sound-frogger-plunk.wav");
	}

	if (m_IsLoadingPlunk && m_FuturePlunk.wait_for(0s) == std::future_status::ready)
	{
		m_IsLoadingPlunk = false;
		m_SoundPlunk = std::static_pointer_cast<Sound>(m_FuturePlunk.get());
	}

	if (m_SoundPlunk)
	{
		if (ImGui::Button("Play Plunk"))
		{
			m_SoundPlunk->Play(0);
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Load Squash") && !m_IsLoadingSquash)
	{
		m_IsLoadingSquash = true;
		m_FutureSquash = RESOURCES.LoadSoundAsync("Audio/sound-frogger-squash.wav");
	}

	if (m_IsLoadingSquash && m_FutureSquash.wait_for(0s) == std::future_status::ready)
	{
		m_IsLoadingSquash = false;
		m_SoundSquash = std::static_pointer_cast<Sound>(m_FutureSquash.get());
	}

	if (m_SoundSquash)
	{
		if (ImGui::Button("Play Squash"))
		{
			m_SoundSquash->Play(0);
		}
	}

}
