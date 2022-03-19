#include "PeterPepper.h"

#include "Components/InputComponent.h"
#include "GameObject.h"
#include "Components/CollisionComponent.h"
#include "Components/SpriteComponent.h"
#include "PPSpriteMovement.h"

void PeterPepper::BeginPlay()
{
	auto input = GetParent()->GetComponent<InputComponent>();
	auto sprite = GetParent()->GetComponent<SpriteComponent>();
	auto collision = GetParent()->GetComponent<CollisionComponent>();
	
	input->BindKeyPressed(SDLK_d, [this] {this->MoveRight(1.f); });
	input->BindKeyPressed(SDLK_a, [this] {this->MoveRight(-1.f); });

	collision->OnOverlap.BindFunction(this, [this] {OnLifeLost.BroadCast(); });
	collision->OnOverlap.BindFunction(input, [input] {input->SetActive(false); });
	sprite->OnAnimationEnd.BindFunction(input, [input] {input->SetActive(true); });

	//// debug press space to die
	//if (collision && input)
	//{
	//	input->BindKeyDown(SDLK_SPACE, [collision] {collision->OnOverlap.BroadCast(); });
	//}

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
