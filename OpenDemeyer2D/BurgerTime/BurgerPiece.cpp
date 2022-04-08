#include "BurgerPiece.h"

#include "GameObject.h"
#include "ResourceManager.h"
#include "Components/TextureComponent.h"
#include "Components/PhysicsComponent.h"
#include "PeterPepper.h"

#include <functional>

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


void BurgerPiece::SegmentOverlap(GameObject* pSegment, PhysicsComponent* other)
{
	if (!other->GetParent()->GetComponent<PeterPepper>()) return;

	for (int i{}; i < 4; ++i)
	{
		if (pSegment == m_pSegments[i] && !m_HitSegments[i])
		{
			m_HitSegments[i] = true;
			m_pSegments[i]->GetTransform()->Move({ 0,-2.f });
			break;
		}
	}
}