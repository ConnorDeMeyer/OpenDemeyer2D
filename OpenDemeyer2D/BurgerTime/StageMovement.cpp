#include "StageMovement.h"

#include "imgui.h"
#include "GameObject.h"

void StageMovement::Move(movementDirection direction)
{
	if (m_pStage && m_pStage->CanMoveInDirection(GetParent()->GetTransform()->GetLocalPosition(), direction))
	{
		switch (direction)
		{
		case movementDirection::right:
			m_MovementInput.x = 1.f;
			break;
		case movementDirection::left:
			m_MovementInput.x = -1.f;
			break;
		case movementDirection::up:
			m_MovementInput.y = 1.f;
			break;
		case movementDirection::down:
			m_MovementInput.y = -1.f;
			break;
		default:
			assert(false);
			break;
		}
	}
}

void StageMovement::Update(float deltaTime)
{
	glm::vec2 movement{ m_MovementInput };

	if (movement.x != 0.f) movement.y = 0.f;

	movement.x *= m_HorizontalMovementSpeed;
	movement.y *= m_VerticalMovementSpeed;
	movement *= deltaTime;
	GetParent()->GetTransform()->Move(movement);


	if (m_MovementInput.x != 0.f)
		m_pStage->SnapToGridY(GetParent()->GetTransform());

	if (m_MovementInput.y != 0.f)
		m_pStage->SnapToGridX(GetParent()->GetTransform());

	m_MovementInput = {};
}

void StageMovement::RenderImGui()
{
	ImGui::InputFloat("Horizontal Speed", &m_HorizontalMovementSpeed);
	ImGui::InputFloat("Vertical Speed", &m_VerticalMovementSpeed);
}