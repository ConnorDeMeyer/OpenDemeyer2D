#include "Score.h"

#include "Singletons/ResourceManager.h"
#include "EngineFiles/Scene.h"
#include "Components/TextPixelComponent.h"
#include "Components/TextureComponent.h"
#include "Components/RenderComponent.h"
#include "Components/Transform.h"

#include "ImGuiExt/imgui_helpers.h"

std::unordered_map<int, float> Score::ScoreTextureMap{
	{50,0.f},
	{100,16.f},
	{200,32.f},
	{300,48.f},
	{500,64.f},
	{1000,80.f},
	{2000,96.f},
	{4000,112.f},
	{8000,128.f},
	{16000,144.f}
};

void Score::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<std::shared_ptr<Texture2D>>("SheetTexture", offsetof(Score, m_SheetTexture));
}

void Score::Update(float deltaTime)
{
	bool changed{};

	while (ScoreEventQueue::HasMessage())
	{
		changed = true;

		auto& message = ScoreEventQueue::PeekMessage();
		ScoreEventQueue::PopMessage();

		ScoreEvent scoreType = std::get<0>(message);
		int score{};
		switch (scoreType)
		{
		case ScoreEvent::drop_Burger:		score = 50;		break;
		case ScoreEvent::kill_hotDog:		score = 100;	break;
		case ScoreEvent::kill_pickle:		score = 200;	break;
		case ScoreEvent::kill_egg:			score = 300;	break;
		case ScoreEvent::drop_burger_1enemy:score = 500;	break;
		case ScoreEvent::drop_burger_2enemy:score = 1000;	break;
		case ScoreEvent::drop_burger_3enemy:score = 2000;	break;
		case ScoreEvent::drop_burger_4enemy:score = 4000;	break;
		case ScoreEvent::drop_burger_5enemy:score = 8000;	break;
		case ScoreEvent::drop_burger_6enemy:score = 16000;	break;
		}

		m_Score += score;

		auto scoreSprite = new GameObject();
		auto render = scoreSprite->AddComponent<RenderComponent>();
		auto texture = scoreSprite->AddComponent<TextureComponent>();

		texture->SetTexture(m_SheetTexture);

		render->SetSourceRect(SDL_FRect{ ScoreTextureMap[score],160.f, 16.f,8.f });
		render->SetRenderAlignMode(eRenderAlignMode::bottomRight);
		render->SetRenderLayer(2);

		scoreSprite->GetTransform()->SetPosition(std::get<1>(message) + glm::vec2{ 8.f, 20.f });
		GetObject()->GetScene()->Add(scoreSprite);

		m_ScoreDisplays.emplace_back(std::pair{ scoreSprite, 2.f });
	}

	if (changed)
	{
		GetObject()->GetComponent<TextPixelComponent>()->SetText(std::to_string(m_Score));
	}

	for (auto& scoreDisplays : m_ScoreDisplays)
	{
		scoreDisplays.second -= deltaTime;
	}
	while (!m_ScoreDisplays.empty() && m_ScoreDisplays.front().second <= 0.f)
	{
		m_ScoreDisplays.front().first->Destroy();
		m_ScoreDisplays.pop_front();
	}
}

void Score::RenderImGui()
{
	ImGui::ResourceSelect("SheetTexture", m_SheetTexture);
}
