//#pragma once
//#include <vector>
//#include <string>
//#include <iostream>

//class Scene;
//class ComponentBase;
//class GameObject;
//class SceneManager;
//
//enum class eInstructions : uint8_t
//{
//	none			= 0x0,
//	pushScene		= 0x1,
//	popScene		= 0x2,
//	pushObject		= 0x3,
//	popObject		= 0x4,
//	pushComponent	= 0x5,
//	popComponent	= 0x6,
//	pushString		= 0x7,
//	popString		= 0x8
//};
//
//void InitiateGame(SceneManager* pSceneManager, const std::string& file);
//
///** has to be binary*/
//void InitiateGame(SceneManager* pSceneManager, std::istream& iStream);
//
////void SaveGame(std::vector<Scene*>& pSceneGraph, const std::string& file);
//
//enum class eComponents : uint32_t
//{
//	none		= 0x0,
//
//	// default Components
//	transform	= 0x1,
//	render		= 0x2,
//	texture		= 0x3,
//	text		= 0x4,
//
//
//	// Custom Components
//	trashTheCache	= 0x8001
//
//};
//
//ComponentBase* AddComponentToObject(GameObject* pObject, eComponents component);