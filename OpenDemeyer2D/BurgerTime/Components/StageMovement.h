#pragma once
#include "EngineFiles/ComponentBase.h"
#include "Stage.h"

class StageMovement : public ComponentBase
{
	COMPONENT_BODY(StageMovement)

public:

	//void SetStage(Stage* pStage) { m_pStage = pStage->GetWeakReferenceType(); }

	bool Move(movementDirection direction);

	// Moves the object without bounds checking
	void MoveUnsafe(movementDirection direction);

	void Update(float) override;

	void RenderImGui() override;

	const glm::vec2& GetMovementInput() const { return m_LastMovementInput; }

	void DefineUserFields(UserFieldBinder&) const override;

protected:

	std::weak_ptr<Stage> m_pStage;

	float m_HorizontalMovementSpeed{32.f};

	float m_VerticalMovementSpeed{32.f};

	glm::vec2 m_MovementInput{};
	glm::vec2 m_LastMovementInput{};

};
