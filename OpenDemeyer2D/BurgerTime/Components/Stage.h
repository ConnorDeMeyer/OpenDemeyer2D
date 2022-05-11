#pragma once
#include "EngineFiles/Component.h"
#include <array>
#include "ResourceWrappers/Texture2D.h"
#include "memory"
#include "glm/glm.hpp"

enum class tiles : char
{
	none = 0,
	platform = 0b01,
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

	void Initialize() override;

	void BeginPlay() override;

	virtual void DefineUserFields(UserFieldBinder&) const;

	bool CanMoveInDirection(const glm::vec2& position, movementDirection direction);

	void SnapToGridX(Transform* transform);

	void SnapToGridY(Transform* transform);

	float GetNextPlatformDown(const glm::vec2& pos);

	void RenderImGui() override;

	void UpdateStageTexture(const std::shared_ptr<Texture2D>& texture);


private:

	void LoadStageTexture();

	SDL_Surface* GenerateStageSurface() const;

	void LoadStageItems();

	void SpawnPlayer();


private:

	std::array<tiles, stageSize> m_Tiles{};

	std::shared_ptr<Texture2D> m_StageTexture;

	GameObject* m_pPeterPepper{};

	Uint8 m_FallenHamburgers[4]{};

	std::vector<int> m_LevelLayout
	{
		1,1,1,1,1,1,1,1,1,
		2,0,2,2,2,0,2,0,2,
		3,1,3,2,3,1,3,1,3,
		0,2,3,3,3,2,2,0,2,
		1,3,3,0,2,2,3,1,3,
		2,2,3,1,3,3,3,2,0,
		2,2,2,0,2,0,3,3,1,
		3,3,3,1,3,1,3,2,2,
		2,0,2,0,2,0,2,2,2,
		3,1,3,1,3,1,3,3,3
	};
};
