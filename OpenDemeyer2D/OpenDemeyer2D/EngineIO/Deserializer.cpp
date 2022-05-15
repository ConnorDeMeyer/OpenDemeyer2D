#include "Deserializer.h"
#include <fstream>

#include "EngineFiles/Scene.h"
#include "EngineFiles/GameObject.h"
#include "Singletons/SceneManager.h"

void Deserializer::RegisterGameObject(unsigned int streamId, GameObject* object)
{
	m_RegisteredObjects.emplace(streamId, object);
}

void Deserializer::GetComponentFromObject(uint32_t objectId, uint32_t compId, std::function<void(std::weak_ptr<ComponentBase>*)> linker)
{
	auto it = m_RegisteredObjects.find(objectId);
	if (it != m_RegisteredObjects.end())
	{
		auto pComp{ it->second->GetComponentById(compId) };
		if (pComp)
		{
			auto weakRef{ pComp->GetWeakReference() };
			linker(&weakRef);
		}
	}
	else // if the object hasn't been parsed yet keep the information inside m_LinkingInfos
	{
		m_LinkingInfos.emplace_back(
			objectId,
			compId,
			linker
		);
	}
}

bool Deserializer::CanContinue()
{
	char buffer{};
	*m_pIStream >> buffer;
	if (buffer != '{' && buffer != '}')
		throw ParsingError("{ or } not found");
	return buffer == '{';
}

bool Deserializer::IsEnd()
{
	if (PeekNextChar(*m_pIStream) == '}')
	{
		char buffer{};
		*m_pIStream >> buffer;
		return true;
	}
	return false;
}

char Deserializer::PeekChar()
{
	char buffer{};
	*m_pIStream >> buffer;
	m_pIStream->unget();
	return buffer;
}

void Deserializer::DeserializeGame(std::istream& iStream)
{
	m_pIStream = &iStream;

	while (PeekNextChar(iStream) == '{')
	{
		while (CanContinue())
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
				auto pComp{ it->second->GetComponentById(link.typeId) };
				if (pComp)
				{
					auto weakRef{ pComp->GetWeakReference() };
					link.pointerLinker(&weakRef);
				}
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
		std::string name;
		iStream >> name;
		pScene = new Scene(name);
		pScene->Deserialize(*this);
	}

	for (auto& link : m_LinkingInfos)
	{
		auto it = m_RegisteredObjects.find(link.objectId);
		if (it != m_RegisteredObjects.end())
		{
			auto pComp{ it->second->GetComponentById(link.typeId) };
			if(pComp)
			{
				auto weakRef{ pComp->GetWeakReference() };
				link.pointerLinker(&weakRef);
			}
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
			auto pComp{ it->second->GetComponentById(link.typeId) };
			if (pComp)
			{
				auto weakRef{ pComp->GetWeakReference() };
				link.pointerLinker(&weakRef);
			}
		}
	}

	m_RegisteredObjects.clear();
	m_LinkingInfos.clear();
	m_pIStream = nullptr;

	return pObject;
}




