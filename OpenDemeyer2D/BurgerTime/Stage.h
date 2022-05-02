#pragma once
#include "Component.h"
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

class Transform;

class Stage final : public Component<Stage>
{
public:
	
	void BeginPlay() override;

	bool CanMoveInDirection(const glm::vec2& position, movementDirection direction);

	void SnapToGridX(Transform* transform);

	void SnapToGridY(Transform* transform);

	float GetNextPlatformDown(const glm::vec2& pos);

	//const std::string GetComponentName() override { return "Stage"; }

	void RenderImGui() override;

private:

	void LoadStageTexture();

	void LoadStageItems();

	void SpawnPlayer();

private:

	std::array<tiles, stageSize> m_Tiles{};

	std::shared_ptr<Texture2D> m_StageTexture;

	GameObject* m_pPeterPepper{};

	Uint8 m_FallenHamburgers[4]{};
};
