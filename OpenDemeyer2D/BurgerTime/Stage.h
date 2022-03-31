#pragma once
#include "ComponentBase.h"
#include <array>
#include <Texture2D.h>
#include "memory"

enum class tiles : char
{
	none = 0,
	platform = 0b1,
	ladder = 0b10,
	ladderPlatform = 0b11
};

constexpr int stageWidth = 9;
constexpr int stageHeight = 10;
constexpr int stageSize = stageWidth * stageHeight;

class Stage final : public ComponentBase
{
public:
	
	void BeginPlay() override;

private:

	void LoadStageTexture();

	void LoadStageItems();

private:

	std::array<tiles, stageSize> m_Tiles{};

	std::shared_ptr<Texture2D> m_StageTexture;
};
