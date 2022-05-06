#include "Enemy.h"
#include "EngineFiles/GameObject.h"
#include "StageMovement.h"
#include "Components/SpriteComponent.h"
#include "Components/Transform.h"
#include "Singletons/ResourceManager.h"

void Enemy::BeginPlay()
{
	m_pStageMovement = GetParent()->GetComponent<StageMovement>();
	m_pSpriteComponent = GetParent()->GetComponent<SpriteComponent>();

	if (m_pSpriteComponent)
	{
		m_pSpriteComponent->SetTexture(RESOURCES.LoadTexture("Bitmaps/FullSheet.png"));
		m_pSpriteComponent->SetTotalFrames(2);
	}

	SetEnemyType(m_EnemyType);
}

void Enemy::Update(float)
{
	UpdateSprite();
}

void Enemy::RenderImGui()
{
}

void Enemy::SetEnemyType(EnemyType type)
{
	m_EnemyType = type;
}

void Enemy::UpdateMovement()
{
	if (m_pStage && m_pTarget && m_pStageMovement)
	{
		auto& pos = GetParent()->GetTransform()->GetLocalPosition();
		auto& targetPos = m_pTarget->GetTransform()->GetLocalPosition();

		bool CanMoveLeft{ m_pStage->CanMoveInDirection(pos, movementDirection::left) };
		bool CanMoveRight{ m_pStage->CanMoveInDirection(pos, movementDirection::right) };
		bool CanMoveUp{ m_pStage->CanMoveInDirection(pos, movementDirection::up) };
		bool CanMoveDown{ m_pStage->CanMoveInDirection(pos, movementDirection::down) };

		// Try to climb ladder first
		if (CanMoveUp || CanMoveDown)
		{
			// If we just came off a ladder
			if (m_ClimbingLadder && (CanMoveLeft || CanMoveRight))
			{

			}

			// if we are already climbing a ladder in a direction, keep going in that direction
			if (m_PreviousMovement.y > 0.1f && CanMoveUp) {
				m_pStageMovement->MoveUnsafe(movementDirection::up);
				m_ClimbingLadder = true;
			}
			else if (m_PreviousMovement.y < 0.1f && CanMoveDown) {
				m_pStageMovement->MoveUnsafe(movementDirection::down);
				m_ClimbingLadder = true;
			}
		}

		//check if we can move horizontally towards the target
		if (targetPos.x < pos.x && CanMoveLeft)
		{
			m_pStageMovement->MoveUnsafe(movementDirection::left);
		}
		else if (targetPos.x >= pos.x && CanMoveRight)
		{
			m_pStageMovement->MoveUnsafe(movementDirection::right);
		}
		else if (CanMoveLeft)
		{
			m_pStageMovement->MoveUnsafe(movementDirection::left);
		}
		m_pStage->CanMoveInDirection(pos, movementDirection::right);






		m_PreviousMovement = m_pStageMovement->GetMovementInput();
	}
}

void Enemy::UpdateSprite()
{
	if (m_pStageMovement)
	{
		auto& movement = m_pStageMovement->GetMovementInput();

		int offset{ 30 + int(m_EnemyType) * 30 };

		if (abs(movement.x) <= 0.05f)
		{
			m_pSpriteComponent->SetFrameOffset(offset);
			m_pSpriteComponent->SetTotalFrames(1);
		}
		else if (movement.x >= 0.05f)
		{
			m_pSpriteComponent->SetFrameOffset(offset + 2);
			m_pSpriteComponent->SetTotalFrames(2);
			GetParent()->GetTransform()->SetScale({ -1,1 });
		}
		else if (movement.x <= -0.05f)
		{
			m_pSpriteComponent->SetTotalFrames(2);
			m_pSpriteComponent->SetFrameOffset(offset + 2);
		}

		if (movement.y >= 0.05f)
		{
			m_pSpriteComponent->SetTotalFrames(2);
			m_pSpriteComponent->SetFrameOffset(offset + 4);
		}
		else if (movement.y <= -0.05f)
		{
			m_pSpriteComponent->SetTotalFrames(2);
			m_pSpriteComponent->SetFrameOffset(offset);
		}
	}
}
