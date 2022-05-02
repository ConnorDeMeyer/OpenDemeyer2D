#include "Deserializer.h"
#include <fstream>

#include <Scene.h>
#include <GameObject.h>
#include <SceneManager.h>

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

void DeserializerBase::RegisterGameObject(unsigned int streamId, GameObject* object)
{
	m_RegisteredObjects.emplace(streamId, object);
}

void DeserializerBase::GetComponentFromObject(std::type_index typeId, unsigned int objectId, ComponentBase** ComponentAddress)
{
	auto it = m_RegisteredObjects.find(objectId);
	if (it != m_RegisteredObjects.end())
	{
		*ComponentAddress = it->second->GetComponentById(typeId);
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

void FileDeserializer::DeserializeGame()
{
	
}

Scene* FileDeserializer::DeserializeScene()
{
	return nullptr;
}

GameObject* FileDeserializer::DeserializeObject()
{
	return nullptr;
}

