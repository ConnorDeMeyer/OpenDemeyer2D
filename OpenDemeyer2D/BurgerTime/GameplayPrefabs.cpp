//
//#include "InputManager.h"
//#include "OpenDemeyer2D.h"
//#include "Components/PeterPepper.h"
//#include "Components/PlayerUI.h"
//#include "Components/PPSpriteMovement.h"
//#include "ResourceManager.h"
//#include "EngineFiles/Scene.h"
//#include "Components/PhysicsComponent.h"
//#include "Components/InputComponent.h"
//#include "Components/RenderComponent.h"
//#include "Components/RenderComponent.h"
//#include "Components/Score.h"
//#include "Components/SpriteComponent.h"
//#include "Components/TextPixelComponent.h"
//#include "Components/Transform.h"
//#include "Components/StageMovement.h"
//#include "GameplayPrefabs.h"
//#include "Enemy.h"
//
//GameObject* PeterPepperFactory()
//{
//	auto PeterPepperObj = new GameObject();
//	PeterPepperObj->AddComponent<PhysicsComponent>();
//	PeterPepperObj->AddComponent<RenderComponent>();
//	PeterPepperObj->AddComponent<InputComponent>();
//	PeterPepperObj->AddComponent<SpriteComponent>();
//	PeterPepperObj->AddComponent<PPSpriteMovement>();
//	PeterPepperObj->AddComponent<PeterPepper>();
//	PeterPepperObj->AddComponent<StageMovement>();
//	return PeterPepperObj;
//}
//
//GameObject* PlayerUiFactory(GameObject* peterPepper)
//{
//	auto PeterPepperComp = peterPepper->GetComponent<PeterPepper>();
//	assert(PeterPepperComp);
//
//	auto playerUI = new GameObject();
//	auto playerUIComp = playerUI->AddComponent<PlayerUI>();
//	playerUIComp->LinkPlayer(PeterPepperComp);
//	playerUI->GetTransform()->SetPosition({ 30,30 });
//	return playerUI;
//}
//
//GameObject* EnemyFactory()
//{
//	auto enemyObj = new GameObject();
//	enemyObj->AddComponent<RenderComponent>();
//	enemyObj->AddComponent<SpriteComponent>();
//	enemyObj->AddComponent<Enemy>();
//	enemyObj->AddComponent<StageMovement>();
//	enemyObj->AddComponent<PhysicsComponent>();
//	return enemyObj;
//}
//
//void AddGameUI(Scene* pScene)
//{
//	{
//		GameObject* UP = new GameObject();
//		auto upRender = UP->AddComponent<RenderComponent>();
//		auto upText = UP->AddComponent<TextPixelComponent>();
//		upRender->SetRenderAlignMode(eRenderAlignMode::topLeft);
//		upText->SetText("1UP");
//		upText->setFontTexture(RESOURCES.LoadSurface("Bitmaps/Text.png"));
//		upText->SetCharPixelSize(8);
//		upText->SetColor({ 255, 0, 0, 255 });
//		UP->GetTransform()->SetPosition({ 32.f, 248.f });
//
//		pScene->Add(UP);
//	}
//	{
//		GameObject* HiScore = new GameObject();
//		auto HiScoreRender = HiScore->AddComponent<RenderComponent>();
//		auto HiScoreText = HiScore->AddComponent<TextPixelComponent>();
//		HiScoreRender->SetRenderAlignMode(eRenderAlignMode::topLeft);
//		HiScoreText->SetText("HI-SCORE");
//		HiScoreText->setFontTexture(RESOURCES.LoadSurface("Bitmaps/Text.png"));
//		HiScoreText->SetCharPixelSize(8);
//		HiScoreText->SetColor({ 255, 0, 0, 255 });
//		HiScore->GetTransform()->SetPosition({ 72.f, 248.f });
//
//		pScene->Add(HiScore);
//	}
//	{
//		GameObject* Pepper = new GameObject();
//		auto PepperRender = Pepper->AddComponent<RenderComponent>();
//		auto PepperText = Pepper->AddComponent<TextPixelComponent>();
//		PepperRender->SetRenderAlignMode(eRenderAlignMode::topLeft);
//		PepperText->SetText("PEPPER");
//		PepperText->setFontTexture(RESOURCES.LoadSurface("Bitmaps/Text.png"));
//		PepperText->SetCharPixelSize(8);
//		PepperText->SetColor({ 0, 255, 0, 255 });
//		Pepper->GetTransform()->SetPosition({ 184.f, 248.f });
//
//		pScene->Add(Pepper);
//	}
//	{
//		GameObject* score = new GameObject();
//		auto render = score->AddComponent<RenderComponent>();
//		auto text = score->AddComponent<TextPixelComponent>();
//		score->AddComponent<Score>();
//		text->setFontTexture(RESOURCES.LoadSurface("Bitmaps/Text.png"));
//		text->SetText("0");
//		score->GetTransform()->SetPosition({ 128.f, 240.f });
//		render->SetRenderAlignMode(eRenderAlignMode::topRight);
//
//		pScene->Add(score);
//	}
//}