#include "PPSpriteMovement.h"

#include "EngineFiles/GameObject.h"
#include "PeterPepper.h"
#include "Components/SpriteComponent.h"
#include "Components/Transform.h"
#include "StageMovement.h"
#include "Singletons/ResourceManager.h"
#include "EngineFiles/ComponentBase.h"

void PPSpriteMovement::BeginPlay()
{
	m_pStageMovement = GetGameObject()->GetComponent<StageMovement>();
	m_pPeterPepper = GetComponent<PeterPepper>();

	m_pSpriteComponent = GetGameObject()->GetComponent<SpriteComponent>();
	if (m_pSpriteComponent)
	{
		//m_pSpriteComponent->SetFrameDimension({ 16,16 });
		//m_pSpriteComponent->SetTotalFrames(3);
		//m_pSpriteComponent->SetTimePerFrame(1.f/8.f);
		m_pSpriteComponent->OnAnimationEnd.BindFunction(this, [this] { this->ResetAnimation(); });
		m_OriginalAnimationSpeed = m_pSpriteComponent->GetTimePerFrame();
	}
	
}

void PPSpriteMovement::Update(float dt)
{
	if (m_IsThrowing)
	{
		m_AccumulatedTime -= dt;
		m_IsThrowing = (m_AccumulatedTime <= 0);
	}

	if (m_pPeterPepper && m_pSpriteComponent && m_pStageMovement && !m_isDying)
	{
		auto& direction = m_pStageMovement->GetMovementInput();

		if (GetGameObject()->GetTransform()->GetLocalScale().x != 1.f)
			GetGameObject()->GetTransform()->SetScale({ 1,1 });

		if (!m_IsThrowing)
		{

			if (abs(direction.x) <= 0.05f) // standing still
			{
				m_pSpriteComponent->SetFrameOffset(1);
				m_pSpriteComponent->SetTotalFrames(1);
			}
			else if (direction.x >= 0.05f) // going right
			{
				m_pSpriteComponent->SetFrameOffset(3);
				m_pSpriteComponent->SetTotalFrames(3);
				GetGameObject()->GetTransform()->SetScale({ -1,1 });
			}
			else if (direction.x <= -0.05f) // going left
			{
				m_pSpriteComponent->SetTotalFrames(3);
				m_pSpriteComponent->SetFrameOffset(3);
			}

			if (direction.y >= 0.05f) // going up
			{
				m_pSpriteComponent->SetTotalFrames(3);
				m_pSpriteComponent->SetFrameOffset(6);
			}
			else if (direction.y <= -0.05f) // going down
			{
				m_pSpriteComponent->SetTotalFrames(3);
				m_pSpriteComponent->SetFrameOffset(0);
			}

		}
		else if (m_IsThrowing)
		{
			m_pSpriteComponent->SetTotalFrames(1);

			if (abs(direction.x) <= 0.05f) // standing still
			{
				m_pSpriteComponent->SetFrameOffset(15);
			}
			else if (direction.x >= 0.05f) // going right
			{
				m_pSpriteComponent->SetFrameOffset(16);
				GetGameObject()->GetTransform()->SetScale({ -1,1 });
			}
			else if (direction.x <= -0.05f) // going left
			{
				m_pSpriteComponent->SetFrameOffset(16);
			}

			if (direction.y >= 0.05f) // going up
			{
				m_pSpriteComponent->SetFrameOffset(17);
			}
			else if (direction.y <= -0.05f) // going down
			{
				m_pSpriteComponent->SetFrameOffset(15);
			}
		}
	}
}

void PPSpriteMovement::StartDieAnimation()
{
	if (m_pSpriteComponent)
	{
		m_pSpriteComponent->SetFrameOffset(18);
		m_pSpriteComponent->SetTotalFrames(6);
		m_pSpriteComponent->SetLooping(false);
		m_pSpriteComponent->SetCurrentFrame(0);
		m_pSpriteComponent->SetTimePerFrame(m_OriginalAnimationSpeed * 3.f);
		m_isDying = true;
	}
}

void PPSpriteMovement::ResetAnimation()
{
	if (m_pSpriteComponent && m_isDying)
	{
		m_pSpriteComponent->SetTotalFrames(3);
		m_pSpriteComponent->SetLooping(true);
		m_pSpriteComponent->SetFrameOffset(0);
		m_pSpriteComponent->Reset();
		m_pSpriteComponent->SetTimePerFrame(m_OriginalAnimationSpeed);
		m_isDying = false;
	}
}

void PPSpriteMovement::StartThrowAnimation()
{
	m_IsThrowing = true;
	m_AccumulatedTime = 0.2f;
}
