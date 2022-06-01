#pragma once

#include "EngineFiles/ComponentBase.h"
#include "ResourceWrappers/Sound.h"
#include <future>

class SoundLoaderTest : public ComponentBase
{
	COMPONENT_BODY(SoundLoaderTest)

public:

	void RenderImGui() override;

private:

	std::vector<std::shared_ptr<Music>> m_Music{};
	std::vector<std::shared_ptr<Sound>> m_Sounds{};

};

