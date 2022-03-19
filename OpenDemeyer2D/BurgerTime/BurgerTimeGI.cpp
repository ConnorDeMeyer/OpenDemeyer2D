#include "BurgerTimeGI.h"
#include "SceneManager.h"
#include "CustomComponents/TrashTheCache.h"
#include "GameObject.h"
#include "OD2pch.h"
#include "Scene.h"
#include "Components/TextureComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/InputComponent.h"
#include "Components/HealthComponent.h"
#include "PlayerResources.h"
#include "ResourceManager.h"
#include "PeterPepper.h"
#include "PPSpriteMovement.h"
#include "Components/CollisionComponent.h"
#include "PlayerUI.h"

#include <steam_api.h>

#include "SteamAPI.h"

void BurgerTimeGI::LoadGame()
{
	std::cout << "Press Z to attack Peter Pepper 1\n";
	std::cout << "Press X to boost Peter Pepper 1\n";
	std::cout << "Press C to attack Peter Pepper 2\n";
	std::cout << "Press V to boost Peter Pepper 2\n";

	{
		auto& scene = SCENES.CreateScene("Test Scene");

		{
			auto PeterPepperObj = new GameObject();
			PeterPepperObj->AddComponent<CollisionComponent>();

			PeterPepperObj->AddComponent<RenderComponent>();
			PeterPepperObj->AddComponent<InputComponent>();
			PeterPepperObj->AddComponent<SpriteComponent>();
			PeterPepperObj->AddComponent<PPSpriteMovement>();
			auto PeterPepperComp = PeterPepperObj->AddComponent<PeterPepper>();
			PeterPepperObj->GetTransform()->SetPosition({ 100,100 });
			scene.Add(PeterPepperObj);

			auto playerUI = new GameObject();
			auto playerUIComp = playerUI->AddComponent<PlayerUI>();
			playerUIComp->LinkPlayer(PeterPepperComp);
			playerUI->GetTransform()->SetPosition({ 30,30 });
			scene.Add(playerUI);

			PeterPepperComp->OnScoreGain.BindFunction(PeterPepperComp, [PeterPepperComp] (int)
				{
					if (PeterPepperComp->GetScore() >= 500)
						ACHIEVEMENTS.SetAchievement("ACH_WIN_ONE_GAME");
				});
			PeterPepperComp->OnScoreGain.BindFunction(PeterPepperComp, [PeterPepperComp](int)
				{
					if (PeterPepperComp->GetScore() >= 1000)
						ACHIEVEMENTS.SetAchievement("ACH_TRAVEL_FAR_ACCUM");
				});
			PeterPepperComp->OnScoreGain.BindFunction(PeterPepperComp, [PeterPepperComp](int)
				{
					if (PeterPepperComp->GetScore() >= 2000)
						ACHIEVEMENTS.SetAchievement("ACH_TRAVEL_FAR_SINGLE");
				});
			PeterPepperComp->OnScoreGain.BindFunction(PeterPepperComp, [PeterPepperComp](int)
				{
					if (PeterPepperComp->GetScore() >= 5000)
						ACHIEVEMENTS.SetAchievement("ACH_WIN_100_GAMES");
				});
				
			// debug kill player 1
			INPUT.GetKeyDownActions(SDLK_z).BindFunction(PeterPepperComp, [PeterPepperComp] {PeterPepperComp->LoseLife(); });
			// debug gain score player 1
			INPUT.GetKeyDownActions(SDLK_x).BindFunction(PeterPepperComp, [PeterPepperComp] {PeterPepperComp->GainScore(50); });
		}
		{
			auto PeterPepperObj = new GameObject();
			PeterPepperObj->AddComponent<CollisionComponent>();

			PeterPepperObj->AddComponent<RenderComponent>();
			PeterPepperObj->AddComponent<InputComponent>();
			PeterPepperObj->AddComponent<SpriteComponent>();
			PeterPepperObj->AddComponent<PPSpriteMovement>();
			auto PeterPepperComp = PeterPepperObj->AddComponent<PeterPepper>();
			PeterPepperObj->GetTransform()->SetPosition({ 200,200 });
			scene.Add(PeterPepperObj);

			auto playerUI = new GameObject();
			auto playerUIComp = playerUI->AddComponent<PlayerUI>();
			playerUIComp->LinkPlayer(PeterPepperComp);
			playerUI->GetTransform()->SetPosition({ 200,30 });
			scene.Add(playerUI);

			PeterPepperComp->OnScoreGain.BindFunction(PeterPepperComp, [PeterPepperComp](int)
				{
					if (PeterPepperComp->GetScore() >= 500)
						ACHIEVEMENTS.SetAchievement("ACH_WIN_ONE_GAME");
				});
			PeterPepperComp->OnScoreGain.BindFunction(PeterPepperComp, [PeterPepperComp](int)
				{
					if (PeterPepperComp->GetScore() >= 1000)
						ACHIEVEMENTS.SetAchievement("ACH_TRAVEL_FAR_ACCUM");
				});
			PeterPepperComp->OnScoreGain.BindFunction(PeterPepperComp, [PeterPepperComp](int)
				{
					if (PeterPepperComp->GetScore() >= 2000)
						ACHIEVEMENTS.SetAchievement("ACH_TRAVEL_FAR_SINGLE");
				});
			PeterPepperComp->OnScoreGain.BindFunction(PeterPepperComp, [PeterPepperComp](int)
				{
					if (PeterPepperComp->GetScore() >= 5000)
						ACHIEVEMENTS.SetAchievement("ACH_WIN_100_GAMES");
				});

			// debug kill player 2
			INPUT.GetKeyDownActions(SDLK_c).BindFunction(PeterPepperComp, [PeterPepperComp] {PeterPepperComp->LoseLife(); });
			// debug gain score player 2
			INPUT.GetKeyDownActions(SDLK_v).BindFunction(PeterPepperComp, [PeterPepperComp] {PeterPepperComp->GainScore(50); });
		}
	}
}
