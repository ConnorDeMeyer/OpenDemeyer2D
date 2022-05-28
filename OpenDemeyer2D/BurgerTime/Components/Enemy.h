#pragma once

#include "EngineFiles/ComponentBase.h"
#include "glm/glm.hpp"
#include "UtilityFiles/StateMachine.h"

class StageMovement;
class SpriteComponent;
class Stage;

enum class EnemyType : char
{
	hotDog	= 0,
	pickle	= 1,
	egg		= 2
};

class Enemy final : public ComponentBase
{
	COMPONENT_BODY(Enemy)

public:

	void DefineUserFields(UserFieldBinder&) const override;

	void Initialize() override;

	void BeginPlay() override;

	void Update(float deltaTime) override;

	void RenderImGui() override;

	void SetEnemyType(EnemyType type);

private:

	void UpdateSprite();

private:

	bool Trans_GoUp();
	bool Trans_GoDown();
	bool Trans_GoRight();
	bool Trans_GoLeft();

	void Update_GoDown(float);
	void Update_GoUp(float);
	void Update_GoRight(float);
	void Update_GoLeft(float);

private:

	std::weak_ptr<SpriteComponent> m_pSpriteComponent{};
	std::weak_ptr<StageMovement> m_pStageMovement{};

	std::weak_ptr<Transform> m_pTarget{};

	EnemyType m_EnemyType{};

	StateMachine m_MovementStateMachine;

};

