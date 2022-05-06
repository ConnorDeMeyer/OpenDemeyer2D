#include "Deserializer.h"
#include <fstream>

#include "EngineFiles/Scene.h"
#include "EngineFiles/GameObject.h"
#include "Singletons/SceneManager.h"

//void DeserializerBase::DeserializeGame(const std::string& file)
//{
//	std::ifstream ifstream{ file };
//	if (ifstream)
//		DeserializeGame(ifstream);
//}
//
//void DeserializerBase::DeserializeGame(std::istream& stream)
//{
//	if (!stream) throw ParsingError("Empty stream received");
//
//	//auto& scenes = SCENES;
//
//	DeserializerBase deserializer{};
//
//	while (stream && CanContinue(stream)) // SCENES
//	{
//		std::string sceneName;
//		stream >> sceneName;
//
//		//auto& scene = scenes.CreateScene(sceneName);
//
//		while (CanContinue(stream)) // OBJECTS
//		{
//			
//		}
//
//
//	}
//}

void Deserializer::RegisterGameObject(unsigned int streamId, GameObject* object)
{
	m_RegisteredObjects.emplace(streamId, object);
}

void Deserializer::GetComponentFromObject(std::type_index typeId, unsigned int objectId, std::weak_ptr<ComponentBase>* ComponentAddress)
{
	auto it = m_RegisteredObjects.find(objectId);
	if (it != m_RegisteredObjects.end())
	{
		*ComponentAddress = it->second->GetComponentById(typeId)->GetWeakReference();
	}
	else // if the object hasn't been parsed yet keep the information inside m_LinkingInfos
	{
		m_LinkingInfos.emplace_back(
			objectId,
			ComponentAddress,
			typeId
		);
	}
}

void Deserializer::DeserializeGame(std::istream& iStream)
{
	m_pIStream = &iStream;

	while (PeekNextChar(iStream) == '{')
	{
		while (CanContinue(*this->GetStream()))
		{
			std::string name;
			iStream >> name;
			auto& scene = SCENES.CreateScene(name);

			scene.Deserialize(*this);
		}
		for (auto& link : m_LinkingInfos)
		{
			auto it = m_RegisteredObjects.find(link.objectId);
			if (it != m_RegisteredObjects.end())
			{
				*link.addressOfReference = it->second->GetComponentById(link.typeId)->GetWeakReference();
			}
		}

		m_RegisteredObjects.clear();
		m_LinkingInfos.clear();
	}

	m_pIStream = nullptr;
}

Scene* Deserializer::DeserializeScene(std::istream& iStream)
{
	m_pIStream = &iStream;

	Scene* pScene{};
	if (iStream.good())
	{
		while (CanContinue(*this->GetStream()))
		{
			std::string name;
			iStream >> name;
			pScene = new Scene(name);
			pScene->Deserialize(*this);
		}
	}

	for (auto& link : m_LinkingInfos)
	{
		auto it = m_RegisteredObjects.find(link.objectId);
		if (it != m_RegisteredObjects.end())
		{
			*link.addressOfReference = it->second->GetComponentById(link.typeId)->GetWeakReference();
		}
	}

	m_RegisteredObjects.clear();
	m_LinkingInfos.clear();
	m_pIStream = nullptr;

	return pScene;
}

GameObject* Deserializer::DeserializeObject(std::istream& iStream)
{
	m_pIStream = &iStream;

	GameObject* pObject{};
	if (iStream.good())
	{
		pObject = new GameObject();
		pObject->Deserialize(*this);
	}

	for (auto& link : m_LinkingInfos)
	{
		auto it = m_RegisteredObjects.find(link.objectId);
		if (it != m_RegisteredObjects.end())
		{
			*link.addressOfReference = it->second->GetComponentById(link.typeId)->GetWeakReference();
		}
	}

	m_RegisteredObjects.clear();
	m_LinkingInfos.clear();
	m_pIStream = nullptr;

	return pObject;
}

//void FileDeserializer::DeserializeGame()
//{
//	if (good())
//	{
//		while (CanContinue(*this))
//		{
//			std::string name;
//			*this >> name;
//			auto& scene = SCENES.CreateScene(name);
//
//			scene.Deserialize(*this);
//		}
//	}
//}



