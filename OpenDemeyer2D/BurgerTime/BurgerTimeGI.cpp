#include "BurgerTimeGI.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "OD2pch.h"
#include "Scene.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/HealthComponent.h"
#include "ResourceManager.h"
#include "Components/PhysicsComponent.h"
#include "Components/TextPixelComponent.h"
#include "GameplayPrefabs.h"
#include "Stage.h"

#include <steam_api.h>

#include "SteamAPI.h"

void BurgerTimeGI::LoadGame()
{
	auto& scene = SCENES.CreateScene("Test Scene");

	AddGameUI(&scene);

	GameObject* level = new GameObject();
	level->AddComponent<RenderComponent>();
	level->AddComponent<Stage>();
	level->GetTransform()->SetPosition({ 24.f,64.f });
	scene.Add(level);
}
