#pragma once
#include "Component.h"
#include "Stage.h"

class StageMovement : public Component<StageMovement>
{
public:


	void SetStage(Stage* pStage) { m_pStage = pStage->GetWeakReferenceType(); }

	bool Move(movementDirection direction);

	// Moves the object without bounds checking
	void MoveUnsafe(movementDirection direction);

	void Update(float) override;

	void LateUpdate() override;

	void RenderImGui() override;

	const glm::vec2& GetMovementInput() const { return m_MovementInput; }

	virtual void DefineUserFields(UserFieldBinder&) const;

protected:

	std::weak_ptr<Stage> m_pStage;

	float m_HorizontalMovementSpeed{32.f};

	float m_VerticalMovementSpeed{32.f};

	glm::vec2 m_MovementInput{};

};
