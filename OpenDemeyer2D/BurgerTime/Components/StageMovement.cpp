#include "StageMovement.h"

#include "imgui.h"
#include "EngineFiles/GameObject.h"
#include "EngineFiles/ComponentBase.h"
#include "Components/Transform.h"
#include "ImGuiExt/imgui_helpers.h"

bool StageMovement::Move(movementDirection direction)
{
	if (!m_pStage.expired() && m_pStage.lock()->CanMoveInDirection(GetGameObject()->GetTransform()->GetLocalPosition(), direction))
	{
		MoveUnsafe(direction);
		return true;
	}
	return false;
}

void StageMovement::MoveUnsafe(movementDirection direction)
{
	if (!m_IsEnabled)
		return;

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

void StageMovement::Update(float deltaTime)
{
	glm::vec2 movement{ m_MovementInput };

	if (movement.x != 0.f) movement.y = 0.f;

	movement.x *= m_HorizontalMovementSpeed;
	movement.y *= m_VerticalMovementSpeed;
	movement *= deltaTime;
	GetGameObject()->GetTransform()->Move(movement);

	if (!m_pStage.expired())
	{
		if (m_MovementInput.x != 0.f)
			m_pStage.lock()->SnapToGridY(GetGameObject()->GetTransform());

		if (m_MovementInput.y != 0.f)
			m_pStage.lock()->SnapToGridX(GetGameObject()->GetTransform());
	}

	m_LastMovementInput = m_MovementInput;
	m_MovementInput = {};
}

void StageMovement::RenderImGui()
{
	ImGui::InputFloat("Horizontal Speed", &m_HorizontalMovementSpeed);
	ImGui::InputFloat("Vertical Speed", &m_VerticalMovementSpeed);

	ImGui::ComponentSelect("Stage", this, m_pStage);
}

void StageMovement::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<std::weak_ptr<Stage>>("Stage", offsetof(StageMovement,m_pStage));
	binder.Add<float>("HorSpeed", offsetof(StageMovement,m_HorizontalMovementSpeed));
	binder.Add<float>("VerSpeed", offsetof(StageMovement,m_VerticalMovementSpeed));
}

void StageMovement::SnapToGridY()
{
	if (auto stage = m_pStage.lock())
	{
		stage->TeleportToNearestGridY(GetTransform());
	}
}
