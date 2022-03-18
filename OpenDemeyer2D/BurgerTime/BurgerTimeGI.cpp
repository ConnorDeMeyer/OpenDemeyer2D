#include "BurgerTimeGI.h"
#include "SceneManager.h"
#include "CustomComponents/TrashTheCache.h"
#include "GameObject.h"
#include "Scene.h"
#include "Components/TextureComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/InputComponent.h"

void BurgerTimeGI::LoadGame()
{
	{
		auto& scene = SCENES.CreateScene("Test Scene");

		{
			auto go = new GameObject();
			go->AddComponent<RenderComponent>();
			go->AddComponent<InputComponent>();

			auto sprite = go->AddComponent<SpriteComponent>();
			sprite->SetTexture("Bitmaps/PlayerWalking.png");
			sprite->SetFrameDimension({ 16,16 });
			sprite->SetFrameOffset(4);

			go->GetTransform()->SetScale({ 10,10 });
			go->GetTransform()->SetPosition({ 500,500 });

			scene.Add(go);
		}
	}
}
