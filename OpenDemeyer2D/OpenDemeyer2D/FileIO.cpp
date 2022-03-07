#include "FileIO.h"
#include "Scene.h"
#include "ComponentBase.h"
#include "GameObject.h"

#include <stack>
#include <fstream>
#include <ios>
#include <map>
#include <functional>
#include <sstream>

#include "Components/RenderComponent.h"
#include "Components/Transform.h"
#include "Components/TextureComponent.h"
#include "Components/TextComponent.h"

#include "CustomComponents/TrashTheCache.h"

/**
 * Game file Notes:
 * - game file is binary
 * - every string should contain the length of the string + 1 (because of the null terminated character)
 *   before the start of the string in 1 byte (max length of the string can only 254 chars)
 *
 * Game file structure:
 * 
 *	pushScene
 *		#name of scene
 *		pushObject
 *			pushComponent
 *				-eComponentType
 *				-initialize component
 *			popComponent
 *			...
 *			pushObject (child object)
 *			...
 *			popObject
 *		popObject
 *		...
 *	popScene
 *
 *	pushScene
 *		...
 *	popScene
 *
 * Component Initialization structure
 * - stream:
 *		length of string -> parameter name -> size in bytes of value (32 bytes) -> value
 *		...
 *	Initialize static dictionary and initialize component with dictionary
 *
 */

/**
 * Component Map used for adding components to 
 */
static std::unordered_map<eComponents, std::function<ComponentBase* (GameObject*)>> ComponentMap
{
	{eComponents::transform,	[](GameObject* o){return o->AddComponent<Transform>(); }},
	{eComponents::render,		[](GameObject* o) {return o->AddComponent<RenderComponent>(); }},
	{eComponents::texture,	[](GameObject* o) {return o->AddComponent<TextureComponent>(); }},
	{eComponents::text,		[](GameObject* o) {return o->AddComponent<TextComponent>(); }}
};

ComponentBase* AddComponentToObject(GameObject* pObject, eComponents component)
{
	return ComponentMap.find(component)->second(pObject);
}

//void InitializeComponents(GameObject* object, std::istream& istream)
//{
//	while (istream.peek() != int(eInstructions::popObject))
//	{
//		uint32_t componentType{};
//		istream >> componentType;
//
//
//	}
//}

void InitiateGame(SceneManager* pSceneManager, const std::string& file)
{
	auto ifstream = std::ifstream(file.c_str(), std::ios::binary);
	InitiateGame(pSceneManager, ifstream);
}

void InitiateGame(SceneManager* pSceneManager, std::istream& iStream)
{
	Scene* pCurrentScene{};
	std::stack<GameObject*> gameObjects;

	while (iStream) {

		uint8_t instruction{};
		iStream >> instruction;

		switch (eInstructions eInstruction{ instruction }) {

		case eInstructions::pushComponent:
		{
			uint32_t componentType{};
			iStream >> componentType;

			auto pComponent = AddComponentToObject(gameObjects.top(), eComponents(componentType));
			auto& classDefault = pComponent->GetClassDefault();

			while (iStream.peek() != int(eInstructions::popComponent))
			{
				uint8_t stringSize;
				iStream >> stringSize;

				std::string key(stringSize, '.');

				uint32_t parameterSize{};
				iStream >> parameterSize;

				std::string parameter = std::string(parameterSize, '.');
				iStream.read(const_cast<char*>(parameter.c_str()), parameterSize);

				auto sstream = std::stringstream(parameter, std::ios::binary | std::ios::out | std::ios::in);

				classDefault.StreamChange(key, sstream);
			}

			pComponent->InitializeComponent(classDefault);

			break;
		}
		case eInstructions::popComponent:
		{


			break;
		}
		case eInstructions::pushObject:
		{
			gameObjects.push(new GameObject());

			break;
		}
		case eInstructions::popObject:
		{
			GameObject* pCurrentObject = gameObjects.top();
			gameObjects.pop();

			if (!gameObjects.empty())
				pCurrentObject->SetParent(gameObjects.top());
			else
				pCurrentScene->Add(pCurrentObject);
			break;
		}
		case eInstructions::pushScene:
		{
			uint8_t stringSize;
			iStream >> stringSize;

			auto sceneName = std::string(stringSize, '.');
			iStream >> sceneName;

			pCurrentScene = &pSceneManager->CreateScene(sceneName);
			break;
		}
		case eInstructions::popScene:
		{
			pCurrentScene = nullptr;
			break;
		}
		default:
				
			break;
		}


	}
}

//void SaveGame(std::vector<Scene*>& pSceneGraph, const std::string& file)
//{
//}


