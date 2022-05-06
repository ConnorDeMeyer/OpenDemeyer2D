#include "PlayerUI.h"

#include "EngineFiles/GameObject.h"
#include "PeterPepper.h"
#include "PlayerResources.h"
#include "Singletons/ResourceManager.h"
#include "Components/TextComponent.h"
#include "Components/Transform.h"
#include "Components/RenderComponent.h"

void PlayerUI::LinkPlayer(PeterPepper* pPeterPepper)
{
	m_pPeterPepper = pPeterPepper;
	if (!pPeterPepper) return;

	// destroy all children
	for (auto child : GetParent()->GetChildren())
	{
		child->Destroy();
	}

	// make counter for the lives
	auto livesCounter = new GameObject();
	auto lifeResources = livesCounter->AddComponent<PlayerResources>();
	auto texture = RESOURCES.LoadTexture("Bitmaps/FullSheet.png");
	lifeResources->SetResourceTexture(texture);
	lifeResources->SetSourceRect({ 200,0,8,8 });
	lifeResources->SetResources(pPeterPepper->GetLives());
	pPeterPepper->OnLifeLost.BindFunction(lifeResources, [lifeResources] {lifeResources->LoseResource(); });
	livesCounter->SetParent(GetParent());

	auto font = RESOURCES.LoadFont("Fonts/advanced_pixel-7.ttf", 24);

	// make counter for score (burgers)
	auto scoreText = new GameObject();
	scoreText->AddComponent<RenderComponent>();
	auto scoreTextComp = scoreText->AddComponent<TextComponent>();
	scoreTextComp->SetFont(font);
	scoreTextComp->SetText("Score:");
	scoreText->GetTransform()->SetPosition({ 0,-10 });
	scoreText->SetParent(GetParent());

	auto score = new GameObject();
	score->AddComponent<RenderComponent>();
	m_pScoreText = score->AddComponent<TextComponent>();
	m_pScoreText->SetText(std::to_string(pPeterPepper->GetScore()));
	m_pScoreText->SetFont(font);
	score->GetTransform()->SetPosition({ 30, -10 });
	score->SetParent(GetParent());
	pPeterPepper->OnScoreGain.BindFunction(this, [this](int) {UpdateScore(); });
}

void PlayerUI::UpdateScore()
{
	if (m_pPeterPepper && m_pScoreText)
	{
		m_pScoreText->SetText(std::to_string(m_pPeterPepper->GetScore()));
	}
}
