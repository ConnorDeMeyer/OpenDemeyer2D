#include "BurgerPiece.h"

#include "EngineFiles/GameObject.h"
#include "Singletons/ResourceManager.h"
#include "Components/TextureComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/Transform.h"
#include "Components/RenderComponent.h"
#include "Enemy.h"
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
			GetObject()->GetTransform()->SetPosition({ pos.x, m_RestHeight });
			m_IsFalling = false;
			m_FallDelay = 0.5f;
			for (int i{}; i < 4; ++i)
			{
				m_HitSegments[i] = false;
			}
		}
	}
}

void BurgerPiece::FallDown(const glm::vec2& location)
{
	if (!m_Stage.expired())
	{
		GetObject()->GetTransform()->Move({ 0,-2.f });

		std::vector<Enemy*> OverlappingEnemies{};

		for (int i{}; i < 4; ++i)
		{
			m_pSegments[i]->GetTransform()->Move({ 0,2 });

			// Get overlapping enemies
			if (auto physics{ m_pSegments[i]->GetComponent<PhysicsComponent>() })
			{
				auto components = physics->GetOverlappingComponents();

				for (auto comp : components)
				{
					auto Object = comp->GetObject();
					if (auto enemy = Object->GetComponent<Enemy>())
					{
						if (std::find(OverlappingEnemies.begin(), OverlappingEnemies.end(), enemy) == OverlappingEnemies.end())
						{
							OverlappingEnemies.emplace_back(enemy);
						}
					}
				}
			}
		}

		m_RestHeight = m_Stage.lock()->GetNextPlatformDown(location, int(OverlappingEnemies.size()) + 1, this);
		m_IsFalling = true;

		for (auto enemy : OverlappingEnemies)
		{
			enemy->FallDown(m_RestHeight);
		}

		//Add score
		switch (OverlappingEnemies.size())
		{
		case 0:
			ScoreEventQueue::AddMessage(ScoreEvent::drop_Burger, GetObject()->GetTransform()->GetWorldPosition());
			break;
		case 1:
			ScoreEventQueue::AddMessage(ScoreEvent::drop_burger_1enemy, GetObject()->GetTransform()->GetWorldPosition());
			break;
		case 2:
			ScoreEventQueue::AddMessage(ScoreEvent::drop_burger_2enemy, GetObject()->GetTransform()->GetWorldPosition());
			break;
		case 3:
			ScoreEventQueue::AddMessage(ScoreEvent::drop_burger_3enemy, GetObject()->GetTransform()->GetWorldPosition());
			break;
		case 4:
			ScoreEventQueue::AddMessage(ScoreEvent::drop_burger_4enemy, GetObject()->GetTransform()->GetWorldPosition());
			break;
		case 5:
			ScoreEventQueue::AddMessage(ScoreEvent::drop_burger_5enemy, GetObject()->GetTransform()->GetWorldPosition());
			break;
		case 6:
			ScoreEventQueue::AddMessage(ScoreEvent::drop_burger_6enemy, GetObject()->GetTransform()->GetWorldPosition());
			break;
		}		
	}
}

void BurgerPiece::FallDown()
{
	FallDown(GetObject()->GetTransform()->GetLocalPosition());
}


void BurgerPiece::SegmentOverlap(GameObject* pSegment, PhysicsComponent* other)
{
	auto otherBurgerPiece = other->GetObject()->GetParent() ? other->GetObject()->GetParent()->GetComponent<BurgerPiece>() : nullptr;
	auto otherPeterPepper = other->GetObject()->GetComponent<PeterPepper>();

	if ((otherPeterPepper || otherBurgerPiece) && (!m_IsFalling && m_FallDelay <= 0.f))
	{
		for (int i{}; i < 4; ++i)
		{
			if (pSegment == m_pSegments[i] && !m_HitSegments[i])
			{
				m_HitSegments[i] = true;
				m_pSegments[i]->GetTransform()->Move({ 0,-2.f });

				for (int j{}; j < 4; ++j)
					if (m_HitSegments[j] != true) return;

				if (otherBurgerPiece)
					return FallDown({ otherBurgerPiece->GetTransform()->GetLocalPosition().x, otherBurgerPiece->m_RestHeight - 2.f });

				FallDown();
				return;
			}
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

	ImGui::InputInt("Burger Stack Pos", &m_PositionInStack, 1, 2);
}

ENUM_ENABLE_STREAMING(BurgerPieceType)

void BurgerPiece::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<std::weak_ptr<Stage>>("Stage", offsetof(BurgerPiece, m_Stage));
	binder.Add<BurgerPieceType>("type", offsetof(BurgerPiece, m_Type));
	binder.Add<int>("stackPos", offsetof(BurgerPiece, m_PositionInStack));
}
