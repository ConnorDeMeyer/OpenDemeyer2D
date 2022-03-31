#include "PeterPepper.h"

#include "Components/InputComponent.h"
#include "GameObject.h"
#include "Components/PhysicsComponent.h"
#include "Components/SpriteComponent.h"
#include "PPSpriteMovement.h"

void PeterPepper::BeginPlay()
{
	auto input = GetParent()->GetComponent<InputComponent>();
	auto sprite = GetParent()->GetComponent<SpriteComponent>();
	auto collision = GetParent()->GetComponent<PhysicsComponent>();
	
	input->BindKeyPressed(SDLK_d, [this] {this->MoveRight(1.f); });
	input->BindKeyPressed(SDLK_a, [this] {this->MoveRight(-1.f); });

	collision->OnOverlap.BindFunction(this, [this](PhysicsComponent*) {OnLifeLost.BroadCast(); });
	collision->OnOverlap.BindFunction(input, [input](PhysicsComponent*) {input->SetActive(false); });
	sprite->OnAnimationEnd.BindFunction(input, [input] {input->SetActive(true); });

}

void PeterPepper::MoveRight(float Value)
{
	m_Direction = { Value, 0 };
	GetParent()->GetTransform()->Move(m_MovementSpeed * m_Direction);
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
