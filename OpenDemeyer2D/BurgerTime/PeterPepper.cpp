#include "PeterPepper.h"

#include "Components/InputComponent.h"
#include "GameObject.h"
#include "Components/PhysicsComponent.h"
#include "Components/SpriteComponent.h"
#include "PPSpriteMovement.h"
#include "StageMovement.h"

void PeterPepper::BeginPlay()
{
	auto input = GetParent()->GetComponent<InputComponent>();
	auto sprite = GetParent()->GetComponent<SpriteComponent>();
	auto collision = GetParent()->GetComponent<PhysicsComponent>();
	auto movement = GetParent()->GetComponent<StageMovement>();
	auto render = GetParent()->GetRenderComponent();

	input->BindKeyPressed(SDLK_d, [movement] {movement->Move(movementDirection::right); });
	input->BindKeyPressed(SDLK_a, [movement] {movement->Move(movementDirection::left); });
	input->BindKeyPressed(SDLK_w, [movement] {movement->Move(movementDirection::up); });
	input->BindKeyPressed(SDLK_s, [movement] {movement->Move(movementDirection::down); });

	collision->OnOverlap.BindFunction(this, [this](PhysicsComponent*) {OnLifeLost.BroadCast(); });
	collision->OnOverlap.BindFunction(input, [input](PhysicsComponent*) {input->SetActive(false); });
	sprite->OnAnimationEnd.BindFunction(input, [input] {input->SetActive(true); });

	collision->SetAsBox(8, 8);

	render->SetRenderLayer(2);
}

void PeterPepper::LoseLife()
{
	--m_Lives;
	OnLifeLost.BroadCast();
}

void PeterPepper::GainScore(int amount)
{
	m_Score += amount;
	OnScoreGain.BroadCast(amount);
}
