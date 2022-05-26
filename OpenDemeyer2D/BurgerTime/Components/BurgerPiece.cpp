#include "BurgerPiece.h"

#include "EngineFiles/GameObject.h"
#include "Singletons/ResourceManager.h"
#include "Components/TextureComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/Transform.h"
#include "Components/RenderComponent.h"
#include "PeterPepper.h"
#include "Score.h"
#include "ImGuiExt/imgui_helpers.h"

#include <functional>

#include "Stage.h"

void BurgerPiece::Initialize()
{
	int i{};
	for (auto child : GetObject()->GetChildren())
	{
		m_pSegments[i++] = child;
	}
}

void BurgerPiece::BeginPlay()
{
	for (int i{}; i < 4; ++i)
	{
		if (!m_pSegments[i])
			continue;

		auto collision = m_pSegments[i]->GetComponent<PhysicsComponent>();

		collision->OnOverlap.BindFunction(this, 
			std::bind(&BurgerPiece::SegmentOverlap, this, m_pSegments[i], std::placeholders::_1));

	}
	m_RestHeight = GetObject()->GetTransform()->GetLocalPosition().y;

	SetType(m_Type);
}

void BurgerPiece::SetType(BurgerPieceType type)
{
	m_Type = type;
	if (!m_pSegments[0]) return;

	for (int i{}; i < 4; ++i)
	{
		if (m_pSegments[i])
		{
			auto texture = m_pSegments[i]->GetRenderComponent();
			texture->SetSourceRect({ 112.f + float(i) * 8.f, 48.f + int(type) * 8.f, 8.f, 8.f });
		}
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
		GetObject()->GetTransform()->Move({ 0,-48.f * deltaTime });
		auto pos = GetObject()->GetTransform()->GetLocalPosition();
		if (pos.y <= m_RestHeight)
		{
			GetObject()->GetTransform()->SetPosition({pos.x, m_RestHeight});
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
	if (!m_Stage.expired())
	{
		GetObject()->GetTransform()->Move({ 0,-2.f });

		for (int i{}; i < 4; ++i)
			m_pSegments[i]->GetTransform()->Move({ 0,2 });

		m_RestHeight = m_Stage.lock()->GetNextPlatformDown(GetObject()->GetTransform()->GetLocalPosition());
		m_IsFalling = true;

		//Add score
		ScoreEventQueue::AddMessage(ScoreEvent::drop_Burger, GetObject()->GetTransform()->GetWorldPosition());
	}
}


void BurgerPiece::SegmentOverlap(GameObject* pSegment, PhysicsComponent* other)
{
	if (!other->GetObject()->GetComponent<PeterPepper>() && !m_IsFalling && m_FallDelay < 0.f &&
		!other->GetObject()->GetParent()->GetComponent<BurgerPiece>()) return;

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

constexpr size_t amountOfPieces{ 6 };
constexpr const char* PiecesNames[amountOfPieces]
{
	"topBun",
	"bottomBun",
	"cheese",
	"meat",
	"tomato",
	"lettuce"
};

void BurgerPiece::RenderImGui()
{
	if (ImGui::BeginCombo("Burger Piece Type", PiecesNames[int(m_Type)]))
	{
		for (int i{}; i < amountOfPieces; ++i)
			if (ImGui::Selectable(PiecesNames[i], i == int(m_Type)))
			{
				SetType(BurgerPieceType(i));
			}
		ImGui::EndCombo();
	}

	ImGui::ComponentSelect("Stage", this, m_Stage);
}

ENUM_ENABLE_STREAMING(BurgerPieceType)

void BurgerPiece::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<std::weak_ptr<Stage>>("Stage", offsetof(BurgerPiece, m_Stage));
	binder.Add<BurgerPieceType>("type", offsetof(BurgerPiece, m_Type));
}
