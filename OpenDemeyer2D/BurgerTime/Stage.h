#pragma once
#include "ComponentBase.h"
#include <array>
#include <Texture2D.h>
#include "memory"
#include "glm/glm.hpp"

enum class tiles : char
{
	none = 0,
	platform = 0b1,
	ladder = 0b10,
	ladderPlatform = 0b11
};

enum class movementDirection : char
{
	none = 0,
	right,
	left,
	up,
	down
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

	void SpawnPlayer();

	bool CanMoveInDirection(const glm::vec2& position, movementDirection direction);

private:

	std::array<tiles, stageSize> m_Tiles{};

	std::shared_ptr<Texture2D> m_StageTexture;
};
