#include "BurgerTimeGI.h"
#include "Singletons/SceneManager.h"
#include "EngineFiles/GameObject.h"
#include "EngineFiles/Scene.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteComponent.h"
#include "Singletons/ResourceManager.h"
#include "Components/PhysicsComponent.h"
#include "Components/TextPixelComponent.h"
#include "Components/Transform.h"
#include "GameplayPrefabs.h"

#include <steam_api.h>

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
