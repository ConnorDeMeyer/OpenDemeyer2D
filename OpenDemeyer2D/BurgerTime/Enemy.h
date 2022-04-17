#pragma once

#include "ComponentBase.h"
#include "glm/glm.hpp"

class StageMovement;
class SpriteComponent;
class Stage;

enum class EnemyType : char
{
	hotDog	= 0,
	pickle	= 1,
	egg		= 2
};

class Enemy final : public Component<Enemy>
{
public:

	void BeginPlay() override;

	void Update(float deltaTime) override;

	//const std::string GetComponentName() override { return "Enemy"; };

	void RenderImGui() override;

	void SetEnemyType(EnemyType type);

	void SetStage(Stage* pStage) { m_pStage = pStage; }

	void SetTarget(GameObject* pTarget) { m_pTarget = pTarget; }

private:

	void UpdateMovement();

	void UpdateSprite();

private:

	StageMovement* m_pStageMovement{};

	SpriteComponent* m_pSpriteComponent{};

	Stage* m_pStage{};

	GameObject* m_pTarget{};

	glm::vec2 m_PreviousMovement{};

	EnemyType m_EnemyType{};

	bool m_ClimbingLadder{};
};

