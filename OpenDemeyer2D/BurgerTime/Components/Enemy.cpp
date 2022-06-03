#include "Enemy.h"
#include "EngineFiles/GameObject.h"
#include "StageMovement.h"

#include "Components/SpriteComponent.h"
#include "Components/RenderComponent.h"
#include "Components/Transform.h"
#include "Components/PhysicsComponent.h"
#include "PeterPepper.h"
#include "BurgerPiece.h"
#include "Score.h"

#include "Singletons/ResourceManager.h"

#include "UtilityFiles/StateMachine.h"

#include "ImGuiExt/imgui_helpers.h"

using namespace std::placeholders;

ENUM_ENABLE_STREAMING(EnemyType)

void Enemy::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<EnemyType>("Type", offsetof(Enemy, m_EnemyType));
	binder.Add<std::weak_ptr<Transform>>("Target", offsetof(Enemy, m_pTarget));
}

void Enemy::Initialize()
{
	if (auto pStageMov = GetComponent<StageMovement>())
		m_pStageMovement = pStageMov->GetWeakReferenceType();
	if (auto pSprite = GetComponent<SpriteComponent>())
		m_pSpriteComponent = pSprite->GetWeakReferenceType();

	UpdateSprite();
}

void Enemy::BeginPlay()
{
	m_StartPos = GetTransform()->GetLocalPosition();

	if (!m_pSpriteComponent.expired())
	{
		auto sprite = m_pSpriteComponent.lock();

		sprite->SetTotalFrames(2);
		sprite->OnAnimationEnd.BindFunction(std::bind(&Enemy::EndOfDyingAnimation, this));
		m_InitialAnimationSpeed = sprite->GetTimePerFrame();
	}

	SetEnemyType(m_EnemyType);

	auto collision = GetComponent<PhysicsComponent>();
	if (collision)
	{
		collision->OnOverlap.BindFunction(this, std::bind(&Enemy::OverlapWithPlayer, this, _1));
		collision->OnOverlap.BindFunction(this, std::bind(&Enemy::OverlapWithBurgerPiece, this, _1));
	}

	// Create the states
	auto State_MoveLeft		= new State();
	auto State_MoveRight	= new State();
	auto State_MoveUp		= new State();
	auto State_MoveDown		= new State();

	// Add the transitions
	State_MoveLeft->AddTransition({ std::bind(&Enemy::Trans_GoUp,	this),	State_MoveUp });
	State_MoveLeft->AddTransition({ std::bind(&Enemy::Trans_GoDown,	this),	State_MoveDown });

	State_MoveRight->AddTransition({std::bind(&Enemy::Trans_GoUp,	this),	State_MoveUp });
	State_MoveRight->AddTransition({std::bind(&Enemy::Trans_GoDown,	this),	State_MoveDown });

	State_MoveDown->AddTransition({ std::bind(&Enemy::Trans_GoRight,this),	State_MoveRight });
	State_MoveDown->AddTransition({ std::bind(&Enemy::Trans_GoLeft,	this),	State_MoveLeft });

	State_MoveUp->AddTransition({	std::bind(&Enemy::Trans_GoRight,this),	State_MoveRight });
	State_MoveUp->AddTransition({	std::bind(&Enemy::Trans_GoLeft,	this),	State_MoveLeft });

	// Set the update functions
	State_MoveLeft->SetUpdateFunction(	std::bind(	&Enemy::Update_GoLeft,	this, _1));
	State_MoveRight->SetUpdateFunction(	std::bind(	&Enemy::Update_GoRight,	this, _1));
	State_MoveDown->SetUpdateFunction(	std::bind(	&Enemy::Update_GoDown,	this, _1));
	State_MoveUp->SetUpdateFunction(	std::bind(	&Enemy::Update_GoUp,	this, _1));

	// Add the states to the state machine
	m_MovementStateMachine.AddState(State_MoveLeft);
	m_MovementStateMachine.AddState(State_MoveRight);
	m_MovementStateMachine.AddState(State_MoveUp);
	m_MovementStateMachine.AddState(State_MoveDown);


	// set the first state
	m_DefaultState = State_MoveUp;
	m_MovementStateMachine.SetCurrentState(m_DefaultState);

	m_MovementStateMachine.SetTransitionDelay(0.1f);
}

bool Enemy::Trans_GoUp()
{
	if (m_pTarget.expired() || m_pStageMovement.expired())
		return false;

	auto targetPos = m_pTarget.lock()->GetLocalPosition();
	float heightDif = targetPos.y - GetTransform()->GetLocalPosition().y;

	return (heightDif > 1.f && m_pStageMovement.lock()->Move(movementDirection::up));
}

bool Enemy::Trans_GoDown()
{
	if (m_pTarget.expired() || m_pStageMovement.expired())
		return false;

	auto targetPos = m_pTarget.lock()->GetLocalPosition();
	float heightDif = targetPos.y - GetTransform()->GetLocalPosition().y;

	return (heightDif < -1.f && m_pStageMovement.lock()->Move(movementDirection::down));
}

bool Enemy::Trans_GoRight()
{
	if (m_pTarget.expired() || m_pStageMovement.expired())
		return false;

	auto targetPos = m_pTarget.lock()->GetLocalPosition();
	float horizontalDif = targetPos.x - GetTransform()->GetLocalPosition().x;

	return (horizontalDif > 0.f && m_pStageMovement.lock()->Move(movementDirection::right));
}

bool Enemy::Trans_GoLeft()
{
	if (m_pTarget.expired() || m_pStageMovement.expired())
		return false;

	auto targetPos = m_pTarget.lock()->GetLocalPosition();
	float horizontalDif = targetPos.x - GetTransform()->GetLocalPosition().x;

	return (horizontalDif < 0.f && m_pStageMovement.lock()->Move(movementDirection::left));
}

void Enemy::Update_GoDown(float)
{
	if (!m_pStageMovement.expired())
		m_pStageMovement.lock()->Move(movementDirection::down);
}

void Enemy::Update_GoUp(float)
{
	if (!m_pStageMovement.expired())
		m_pStageMovement.lock()->Move(movementDirection::up);
}

void Enemy::Update_GoRight(float)
{
	if (!m_pStageMovement.expired())
		m_pStageMovement.lock()->Move(movementDirection::right);
}

void Enemy::Update_GoLeft(float)
{
	if (!m_pStageMovement.expired())
		m_pStageMovement.lock()->Move(movementDirection::left);
}

void Enemy::Update(float dt)
{
	m_StunTime -= dt;

	if (m_IsFalling)
	{
		GetTransform()->Move({ 0,-48.f * dt });
		auto pos = GetObject()->GetTransform()->GetLocalPosition();
		if (pos.y <= m_RestLocation)
		{
			GetObject()->GetTransform()->SetPosition({ pos.x, m_RestLocation });
			m_IsFalling = false;
			if (auto stageMovement = m_pStageMovement.lock())
			{
				stageMovement->SnapToGridY();
				m_MovementStateMachine.SetCurrentState(m_DefaultState);
			}
		}
	}
	else if (m_IsStunned)
	{
		if (m_StunTime <= 0.f)
		{
			m_IsStunned = false;
		}
	}
	else if (!m_IsDying)
	{
		m_MovementStateMachine.Update(dt);
		UpdateSprite();
	}
}

constexpr const char* enemyTypes[]
{
	"HotDog",
	"Pickle",
	"Egg"
};

void Enemy::RenderImGui()
{
	ImGui::ComponentSelect("Target", this, m_pTarget);

	if (ImGui::BeginCombo("Enemy Type", enemyTypes[int(m_EnemyType)]))
	{
		for (int i{}; i < sizeof(enemyTypes)/sizeof(const char*); ++i)
			if (ImGui::Selectable(enemyTypes[i], i == int(m_EnemyType)))
			{
				SetEnemyType(EnemyType(i));
			}
		ImGui::EndCombo();
	}
}

void Enemy::SetEnemyType(EnemyType type)
{
	m_EnemyType = type;
	UpdateSprite();
}

void Enemy::UpdateSprite()
{
	if (!m_pStageMovement.expired() && !m_pSpriteComponent.expired())
	{
		auto& movement = m_pStageMovement.lock()->GetMovementInput();
		auto sprite = m_pSpriteComponent.lock();

		int offset{ 30 + int(m_EnemyType) * 30 };

		if (abs(movement.x) <= 0.05f)
		{
			sprite->SetFrameOffset(offset);
			sprite->SetTotalFrames(1);
		}
		else if (movement.x >= 0.05f)
		{
			sprite->SetFrameOffset(offset + 2);
			sprite->SetTotalFrames(2);
			GetObject()->GetTransform()->SetScale({ -1,1 });
		}
		else if (movement.x <= -0.05f)
		{
			sprite->SetTotalFrames(2);
			sprite->SetFrameOffset(offset + 2);
		}

		if (movement.y >= 0.05f)
		{
			sprite->SetTotalFrames(2);
			sprite->SetFrameOffset(offset + 4);
		}
		else if (movement.y <= -0.05f)
		{
			sprite->SetTotalFrames(2);
			sprite->SetFrameOffset(offset);
		}
	}
}

void Enemy::OverlapWithPlayer(PhysicsComponent* other)
{
	if (!m_IsDying && !m_IsStunned)
		if (auto peterPepper{ other->GetObject()->GetComponent<PeterPepper>() })
		{
			peterPepper->LoseLife();
		}
}

void Enemy::OverlapWithBurgerPiece(PhysicsComponent* other)
{
	if (auto parentObject{ other->GetObject()->GetParent() })
		if (auto burgerPiece{ parentObject->GetComponent<BurgerPiece>() })
			if (burgerPiece->IsFalling() && !m_IsFalling)
				Die();
}

void Enemy::EndOfDyingAnimation()
{
	if (m_IsDying)
	{
		m_IsDying = false;

		GetTransform()->SetPosition(m_StartPos);

		if (auto sprite = m_pSpriteComponent.lock())
		{
			sprite->SetLooping(true);
			sprite->SetPaused(false);
			sprite->SetTimePerFrame(m_InitialAnimationSpeed);
		}

		m_MovementStateMachine.SetCurrentState(m_DefaultState);

	}
}

void Enemy::Die()
{
	if (!m_IsDying)
	{
		m_IsDying = true;

		if (!m_pSpriteComponent.expired())
		{
			auto sprite = m_pSpriteComponent.lock();

			sprite->SetFrameOffset(int(m_EnemyType) * 30 + 45);
			sprite->SetTotalFrames(4);
			sprite->SetLooping(false);
			sprite->SetTimePerFrame(m_InitialAnimationSpeed * 2.f);
			sprite->Reset();
		}

		switch (m_EnemyType)
		{
		case EnemyType::hotDog:
			ScoreEventQueue::AddMessage(ScoreEvent::kill_hotDog, GetObject()->GetTransform()->GetWorldPosition());
			break;
		case EnemyType::pickle:
			ScoreEventQueue::AddMessage(ScoreEvent::kill_pickle, GetObject()->GetTransform()->GetWorldPosition());
			break;
		case EnemyType::egg:
			ScoreEventQueue::AddMessage(ScoreEvent::kill_egg, GetObject()->GetTransform()->GetWorldPosition());
			break;
		}
	}
}

void Enemy::FallDown(float location)
{
	m_IsFalling = true;
	m_RestLocation = location;
}

void Enemy::Stun(float time)
{
	if (!m_IsDying && !m_IsStunned)
	{
		m_IsStunned = true;
		m_StunTime = time;

		if (!m_pSpriteComponent.expired())
		{
			auto sprite = m_pSpriteComponent.lock();

			sprite->SetFrameOffset(int(m_EnemyType) * 30 + 49);
			sprite->SetTotalFrames(2);
			sprite->SetLooping(true);
			sprite->Reset();
		}
	}
}
