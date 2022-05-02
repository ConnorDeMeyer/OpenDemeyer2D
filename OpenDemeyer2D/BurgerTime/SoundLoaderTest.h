#pragma once

#include "Component.h"
#include "Sound.h"
#include <future>

class SoundLoaderTest : public Component<SoundLoaderTest>
{
public:

	void RenderImGui() override;

private:

	std::shared_ptr<Sound> m_SoundHop;
	std::shared_ptr<Sound> m_SoundPlunk;
	std::shared_ptr<Sound> m_SoundSquash;

	std::shared_ptr<Music> m_MusicMyAshes;
	std::shared_ptr<Music> m_MusicValkerie;

	bool m_IsLoadingHop{};
	bool m_IsLoadingPlunk{};
	bool m_IsLoadingSquash{};
	bool m_IsLoadingMyAshes{};
	bool m_IsLoadingValkerie{};

	std::future<std::shared_ptr<void>> m_FutureMyAshes;
	std::future<std::shared_ptr<void>> m_FutureValkerie;
	std::future<std::shared_ptr<void>> m_FutureHop;
	std::future<std::shared_ptr<void>> m_FuturePlunk;
	std::future<std::shared_ptr<void>> m_FutureSquash;

};

