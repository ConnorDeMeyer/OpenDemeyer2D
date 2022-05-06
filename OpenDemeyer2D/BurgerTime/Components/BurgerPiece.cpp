#include "BurgerPiece.h"

#include "EngineFiles/GameObject.h"
#include "Singletons/ResourceManager.h"
#include "Components/TextureComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/Transform.h"
#include "Components/RenderComponent.h"
#include "PeterPepper.h"
#include "Score.h"

#include <functional>

#include "Stage.h"

void BurgerPiece::BeginPlay()
{
	auto sheetTexture = RESOURCES.LoadTexture("Bitmaps/FullSheet.png");

	for (int i{}; i < 4; ++i)
	{
		auto pSegment{ new GameObject() };
		auto render = pSegment->AddComponent<RenderComponent>();
		auto texture = pSegment->AddComponent<TextureComponent>();
		auto collision = pSegment->AddComponent<PhysicsComponent>();

		pSegment->GetTransform()->SetPosition({ -12.f + 8.f * float(i), 0 });

		pSegment->SetParent(GetParent());
		render->SetRenderLayer(1);
		render->SetPivot({ 0.5f,0.5f });
		texture->SetTexture(sheetTexture);

		collision->AddBox(3, 3, true);
		collision->OnOverlap.BindFunction(this, 
			std::bind(&BurgerPiece::SegmentOverlap, this, pSegment, std::placeholders::_1));

		m_pSegments[i] = pSegment;

		m_RestHeight = GetParent()->GetTransform()->GetLocalPosition().y;
	}

	SetType(m_Type);
}

void BurgerPiece::SetType(BurgerPieceType type)
{
	m_Type = type;
	if (!m_pSegments[0]) return;

	for (int i{}; i < 4; ++i)
	{
		auto texture = m_pSegments[i]->GetComponent<TextureComponent>();
		texture->SetSourceRect({ 112.f + float(i) * 8.f, 48.f + int(type) * 8.f, 8.f, 8.f });
	}
}

void BurgerPiece::Update(float deltaTime)
{
	if (m_FallDelay >= 0.f)
	{
		m_FallDelay -= deltaTime;
		return;
	}
	if (m_IsFalling)
	{
		GetParent()->GetTransform()->Move({ 0,-48.f * deltaTime });
		auto pos = GetParent()->GetTransform()->GetLocalPosition();
		if (pos.y <= m_RestHeight)
		{
			GetParent()->GetTransform()->SetPosition({pos.x, m_RestHeight});
			m_IsFalling = false;
			m_FallDelay = 0.5f;
			for (int i{}; i < 4; ++i)
			{
				m_HitSegments[i] = false;
			}
		}
	}
}

void BurgerPiece::FallDown()
{
	if (auto pStage = GetParent()->GetParent()->GetComponent<Stage>())
	{
		GetParent()->GetTransform()->Move({ 0,-2.f });

		for (int i{}; i < 4; ++i)
			m_pSegments[i]->GetTransform()->Move({ 0,2 });

		m_RestHeight = pStage->GetNextPlatformDown(GetParent()->GetTransform()->GetLocalPosition());
		m_IsFalling = true;

		//Add score
		ScoreEventQueue::AddMessage(ScoreEvent::drop_Burger, GetParent()->GetTransform()->GetWorldPosition());
	}
}


void BurgerPiece::SegmentOverlap(GameObject* pSegment, PhysicsComponent* other)
{
	if (!other->GetParent()->GetComponent<PeterPepper>() && !m_IsFalling && m_FallDelay < 0.f &&
		!other->GetParent()->GetParent()->GetComponent<BurgerPiece>()) return;

	for (int i{}; i < 4; ++i)
	{
		if (pSegment == m_pSegments[i] && !m_HitSegments[i])
		{
			m_HitSegments[i] = true;
			m_pSegments[i]->GetTransform()->Move({ 0,-2.f });

			for (int j{}; j < 4; ++j)
				if (m_HitSegments[j] != true) return;

			FallDown();
			return;
		}
	}
}
