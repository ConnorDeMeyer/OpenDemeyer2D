#pragma once
#include "ComponentBase.h"
#include "Stage.h"

class StageMovement : public ComponentBase
{
public:


	void SetStage(Stage* pStage) { m_pStage = pStage; }

	void Move(movementDirection direction);

	void Update(float) override;

	void RenderImGui() override;

	const std::string GetComponentName() override { return "StageMovement"; }

protected:

	Stage* m_pStage{};

	float m_HorizontalMovementSpeed{32.f};

	float m_VerticalMovementSpeed{32.f};

	glm::vec2 m_MovementInput{};

};
