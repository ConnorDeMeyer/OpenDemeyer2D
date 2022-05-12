#include "PPSpriteMovement.h"

#include "EngineFiles/GameObject.h"
#include "PeterPepper.h"
#include "Components/SpriteComponent.h"
#include "Components/InputComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/RenderComponent.h"
#include "Components/Transform.h"
#include "StageMovement.h"
#include "Singletons/ResourceManager.h"

void PPSpriteMovement::BeginPlay()
{
	m_pStageMovement = GetParent()->GetComponent<StageMovement>();

	m_pPeterPepper = GetParent()->GetComponent<PeterPepper>();
	if (m_pPeterPepper)
	{
		m_pPeterPepper->OnLifeLost.BindFunction(this, [this] {this->StartDieAnimation(); });
	}

	m_pSpriteComponent = GetParent()->GetComponent<SpriteComponent>();
	if (m_pSpriteComponent)
	{
		m_pSpriteComponent->SetFrameDimension({ 16,16 });
		m_pSpriteComponent->SetTotalFrames(3);
		m_pSpriteComponent->SetTimePerFrame(1.f/8.f);
		m_pSpriteComponent->OnAnimationEnd.BindFunction(this, [this] { this->ResetAnimation(); });
	}
	
	auto pRender = GetRenderComponent();
	pRender->SetPivot({ 0.5f,1.f });
	pRender->SetTexture(RESOURCES.LoadTexture("Data/Bitmaps/FullSheet.png"));
}

void PPSpriteMovement::Update(float)
{
	if (m_pPeterPepper && m_pSpriteComponent && m_pStageMovement)
	{
		auto& direction = m_pStageMovement->GetMovementInput();

		if (GetParent()->GetTransform()->GetLocalScale().x != 1.f)
			GetParent()->GetTransform()->SetScale({ 1,1 });

		if (abs(direction.x) <= 0.05f)
		{
			m_pSpriteComponent->SetFrameOffset(1);
			m_pSpriteComponent->SetTotalFrames(1);
		}
		else if (direction.x >= 0.05f)
		{
			m_pSpriteComponent->SetFrameOffset(3);
			m_pSpriteComponent->SetTotalFrames(3);
			GetParent()->GetTransform()->SetScale({ -1,1 });
		}
		else if (direction.x <= -0.05f)
		{
			m_pSpriteComponent->SetTotalFrames(3);
			m_pSpriteComponent->SetFrameOffset(3);
		}

		if (direction.y >= 0.05f)
		{
			m_pSpriteComponent->SetTotalFrames(3);
			m_pSpriteComponent->SetFrameOffset(6);
		}
		else if (direction.y <= -0.05f)
		{
			m_pSpriteComponent->SetTotalFrames(3);
			m_pSpriteComponent->SetFrameOffset(0);
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
