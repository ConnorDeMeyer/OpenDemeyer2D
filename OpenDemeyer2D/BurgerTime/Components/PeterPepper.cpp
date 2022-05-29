#include "PeterPepper.h"

#include "Components/InputComponent.h"
#include "EngineFiles/GameObject.h"
#include "Components/PhysicsComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/RenderComponent.h"
#include "PPSpriteMovement.h"
#include "StageMovement.h"

void PeterPepper::BeginPlay()
{
	auto input = GetObject()->GetComponent<InputComponent>();
	auto movement = GetObject()->GetComponent<StageMovement>();

	input->BindKeyPressed(SDLK_d, [movement] {movement->Move(movementDirection::right); });
	input->BindKeyPressed(SDLK_a, [movement] {movement->Move(movementDirection::left); });
	input->BindKeyPressed(SDLK_w, [movement] {movement->Move(movementDirection::up); });
	input->BindKeyPressed(SDLK_s, [movement] {movement->Move(movementDirection::down); });

	if (auto sprite = GetObject()->GetComponent<SpriteComponent>())
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

void PeterPepper::StunEnd()
{
	if (m_isStunned)
	{
		m_isStunned = false;

		if (auto movement = GetComponent<StageMovement>())
			movement->SetEnabled(true);
	}
}
