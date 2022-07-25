#include "PeterPepper.h"

#include "Components/InputComponent.h"
#include "EngineFiles/GameObject.h"
#include "Components/PhysicsComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Transform.h"
#include "PPSpriteMovement.h"
#include "StageMovement.h"
#include "ResourceWrappers/Prefab.h"

#include "ImGuiExt/imgui_helpers.h"

void PeterPepper::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<std::shared_ptr<Prefab>>("PepperPrefab", offsetof(PeterPepper, m_PepperPrefab));
}

void PeterPepper::BeginPlay()
{
	if (auto movement = GetGameObject()->GetComponent<StageMovement>())
		if (auto input = GetGameObject()->GetComponent<InputComponent>())
		{
			if (input->GetControllerId() == -1) // keyboard input
			{
				input->BindKeyPressed(SDLK_d, [movement] {movement->Move(movementDirection::right); });
				input->BindKeyPressed(SDLK_a, [movement] {movement->Move(movementDirection::left); });
				input->BindKeyPressed(SDLK_w, [movement] {movement->Move(movementDirection::up); });
				input->BindKeyPressed(SDLK_s, [movement] {movement->Move(movementDirection::down); });
				input->BindKeyDown(SDLK_SPACE, std::bind(&PeterPepper::ThrowPepper, this));
			}
			else // controller input
			{
				input->BindControllerAxis(SDL_CONTROLLER_AXIS_LEFTX, [movement](float axis)
					{
						if (axis < -0.1f)
							movement->Move(movementDirection::left);
						if (axis > 0.1f)
							movement->Move(movementDirection::right);
					});
				input->BindControllerAxis(SDL_CONTROLLER_AXIS_LEFTY, [movement](float axis)
					{
						if (axis > 0.1f)
							movement->Move(movementDirection::down);
						if (axis < -0.1f)
							movement->Move(movementDirection::up);
					});
				input->BindControllerDown(SDL_CONTROLLER_BUTTON_A, std::bind(&PeterPepper::ThrowPepper, this));
			}
		}

	if (auto sprite = GetGameObject()->GetComponent<SpriteComponent>())
	{
		sprite->OnAnimationEnd.BindFunction(this, std::bind(&PeterPepper::StunEnd, this));
	}
}

void PeterPepper::LoseLife()
{
	if (!m_isStunned)
	{
		m_isStunned = true;

		--m_Lives;
		OnLifeLost.BroadCast();

		if (auto movement = GetComponent<StageMovement>())
			movement->SetEnabled(false);

		if (auto spriteMovement = GetComponent<PPSpriteMovement>())
		{
			spriteMovement->StartDieAnimation();
		}
	}
}

void PeterPepper::RenderImGui()
{
	ImGui::ResourceSelect("Pepper Prefab", m_PepperPrefab);
}

void PeterPepper::StunEnd()
{
	if (m_isStunned)
	{
		m_isStunned = false;

		if (auto movement = GetComponent<StageMovement>())
			movement->SetEnabled(true);
	}
}

void PeterPepper::ThrowPepper()
{
	if (m_PepperPrefab && m_Pepper > 0)
	{
		--m_Pepper;

		OnPepperSpray.BroadCast();

		if (auto sprite = GetComponent<PPSpriteMovement>())
		{
			sprite->StartThrowAnimation();
		}

		glm::vec2 direction{};
		if (auto movement = GetComponent<StageMovement>())
		{
			direction = movement->GetMovementInput();
		}

		if (direction != glm::vec2{0, 0})
		{
			direction = glm::normalize(direction);
			constexpr float range{ 8.f };
			direction *= range;
		}

		auto pPepper = m_PepperPrefab->Instantiate(GetScene());
		pPepper->GetTransform()->SetPosition(GetTransform()->GetWorldPosition() + direction);
	}
}
