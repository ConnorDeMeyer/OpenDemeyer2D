#pragma once

class GameObject;
class Scene;

GameObject* PeterPepperFactory();

GameObject* PlayerUiFactory(GameObject* peterPepper);

void AddGameUI(Scene* pScene);