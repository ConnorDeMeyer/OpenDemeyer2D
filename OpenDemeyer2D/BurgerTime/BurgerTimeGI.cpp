﻿#include "BurgerTimeGI.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "OD2pch.h"
#include "Scene.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteComponent.h"
#include "HealthComponent.h"
#include "ResourceManager.h"
#include "Components/PhysicsComponent.h"
#include "Components/TextPixelComponent.h"
#include "Components/Transform.h"
#include "GameplayPrefabs.h"
#include "Stage.h"
#include "SoundLoaderTest.h"

#include <steam_api.h>

#include "SteamAPI.h"
#include "EngineIO/Deserializer.h"


void BurgerTimeGI::LoadGame()
{
	//for (auto pair : TypeInformation::GetInstance().ClassNameIds)
	//{
	//	std::cout << pair.first << ": " << std::to_string(pair.second.hash_code()) <<'\n';
	//}
	//
	//auto& scene = SCENES.CreateScene("Test_Scene");
	//
	//AddGameUI(&scene);
	//
	//GameObject* level = new GameObject();
	//level->AddComponent<RenderComponent>();
	//level->AddComponent<Stage>();
	//level->GetTransform()->SetPosition({ 24.f,64.f });
	//scene.Add(level);
	//
	//auto audioLoaderTest = new GameObject();
	//audioLoaderTest->AddComponent<SoundLoaderTest>();
	//scene.Add(audioLoaderTest);

	auto stream = std::ifstream("BurgerTime");
	Deserializer gameLoader{};
	gameLoader.DeserializeGame(stream);
}
