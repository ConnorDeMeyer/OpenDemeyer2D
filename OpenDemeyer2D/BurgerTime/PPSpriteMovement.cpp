#include "PPSpriteMovement.h"

#include "GameObject.h"
#include "PeterPepper.h"
#include "Components/SpriteComponent.h"
#include "Components/InputComponent.h"
#include "Components/PhysicsComponent.h"

void PPSpriteMovement::BeginPlay()
{
	m_pPeterPepper = GetParent()->GetComponent<PeterPepper>();

	if (m_pPeterPepper)
	{
		m_pPeterPepper->OnLifeLost.BindFunction(this, [this] {this->StartDieAnimation(); });
	}

	m_pSpriteComponent = GetParent()->GetComponent<SpriteComponent>();
	if (m_pSpriteComponent)
	{
		m_pSpriteComponent->SetTexture("Bitmaps/FullSheet.png");
		m_pSpriteComponent->SetFrameDimension({ 16,16 });
		m_pSpriteComponent->SetTotalFrames(3);
		m_pSpriteComponent->OnAnimationEnd.BindFunction(this, [this] { this->ResetAnimation(); });
	}
}

void PPSpriteMovement::Update(float)
{
	if (m_pPeterPepper && m_pSpriteComponent)
	{
		auto& direction = m_pPeterPepper->GetDirection();

		if (direction.x >= 0.2f)
		{
			m_pSpriteComponent->SetFrameOffset(3);
		}
		else if (direction.x <= -0.2f)
		{
			m_pSpriteComponent->SetFrameOffset(3);
			GetParent()->GetTransform()->SetScale({ -1,1 });
		}
	}
}

void PPSpriteMovement::StartDieAnimation()
{
	if (m_pSpriteComponent)
	{
		m_pSpriteComponent->SetFrameOffset(17);
		m_pSpriteComponent->SetTotalFrames(6);
		m_pSpriteComponent->SetLooping(false);
		m_pSpriteComponent->SetCurrentFrame(0);
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
		m_isDying = false;
	}
}
