#include "pch.h"
#include "EngineFiles/Scene.h"

#include "EngineFiles/GameObject.h"
#include "imgui.h"
#include "Components/PhysicsComponent.h"
#include "PhysicsInterface.h"

#include "Singletons/GUIManager.h"

Scene::Scene(const std::string& name)
	: m_Name{ name }
	, m_PhysicsInterface{new PhysicsInterface()}
{
}

Scene::~Scene()
{
	for (auto obj : m_SceneTree)
		delete obj;

	for (auto obj : m_NewSceneTreeObjects)
		delete obj;
}

//GameObject* Scene::Add(GameObject* pObject)
//{
//	if (pObject->m_pScene) throw ObjectAlreadyInASceneException();
//
//	m_UninitializedObject.emplace_back(pObject);
//
//	return pObject;
//}

GameObject* Scene::CreateGameObject(GameObject* pParent)
{
	GameObject* pObject = new GameObject();
	pObject->m_pScene = this;
	RegisterObject(pObject);

	if (pParent)
	{
		pObject->m_Parent = pParent;
		pParent->m_Children.emplace_back(pObject);
	}
	else
	{
		m_NewSceneTreeObjects.emplace_back(pObject);
	}

	m_UninitializedObject.emplace_back(pObject);

	return pObject;
}

void Scene::DestroyObject(GameObject* pObject)
{
	m_DestroyableObjects.emplace_back(pObject);
}

void Scene::DestroyObjectImmediately(GameObject* pObject)
{
	pObject->SetParent(nullptr);
	delete pObject;
	m_SceneTree.SwapRemove(pObject);
}

void Scene::Update(float deltaTime)
{
	if (!m_HasBegunPlay)
		return;

	// Update every object
	for (GameObject* child : m_SceneTree)
	{
		child->Update(deltaTime);
	}

	// Do a late update call for every object
	for (GameObject* child : m_SceneTree)
	{
		child->LateUpdate();
	}
}

void Scene::PreUpdate(bool IsPlaying)
{
	// initialized objects
	for (GameObject* pObject : m_UninitializedObject)
	{
		//m_SceneTree.emplace_back(pObject);
		m_NotBegunObjects.emplace_back(pObject);
		pObject->InitializeComponents();
	}
	m_UninitializedObject.clear();

	for (GameObject* pObject : m_NewSceneTreeObjects)
	{
		m_SceneTree.emplace_back(pObject);
	}
	m_NewSceneTreeObjects.clear();

	if (IsPlaying)
	{
		m_HasBegunPlay = true;

		for (GameObject* pObject : m_NotBegunObjects)
		{
			pObject->BeginPlay();
		}
		m_NotBegunObjects.clear();
	}
}

void Scene::AfterUpdate()
{

	// Delete the destroyed Objects
	for (GameObject* object : m_DestroyableObjects)
	{
		DestroyObjectImmediately(object);
	}
	m_DestroyableObjects.clear();
}

void Scene::Render() const
{
	for (GameObject* child : m_SceneTree)
	{
		child->Render();
	}
}

void Scene::ChangeName(const std::string& name)
{
	auto& scenes = SCENES.GetScenes();

	for (auto& scene : scenes)
	{
		if (scene->GetName() == name)
			return;
	}

	m_Name = name;

}

void Scene::Serialize(std::ostream& os) const
{
	os << m_Name << "\n";
	{
		os << "{\n";
		for (GameObject* pObject : m_SceneTree)
		{
			pObject->Serialize(os);
		}
		os << "}\n";
	}
}

void Scene::Deserialize(Deserializer& is)
{
	if (CanContinue(*is.GetStream()))
	{
		while (!IsEnd(*is.GetStream()))
		{
			auto go = CreateGameObject();
			go->Deserialize(is);
		}
	}
}

void Scene::Copy(Scene* originalScene)
{
	m_SceneTree.reserve(originalScene->m_SceneTree.size());

	CopyLinker linker{};
	linker.SetIsSameScene(false);

	for (auto pObject : originalScene->m_SceneTree)
	{
		auto go = CreateGameObject();
		go->Copy(pObject, &linker);
	}

	linker.PerformLinkingActions();
}

void Scene::AddToSceneTree(GameObject* go)
{
#ifdef _DEBUG
	for (auto sgo : m_SceneTree)
	{
		if (sgo == go)
			assert(false);
	}
#endif // _DEBUG

	m_SceneTree.push_back(go);
}

void Scene::RegisterObject(GameObject* pObject)
{
	pObject->m_ObjectId = ++m_RegistrationCounter;
	m_RegisteredObjects.insert({ m_RegistrationCounter, pObject });
}

void Scene::UnregisterObject(GameObject* pObject)
{
	m_RegisteredObjects.erase(pObject->m_ObjectId);
	pObject->m_ObjectId = 0;

	for (auto child : pObject->GetChildren())
	{
		UnregisterObject(child);
	}
}

void Scene::RemoveObject(GameObject* object)
{
	m_SceneTree.RSwapRemove(object);
}

//void Scene::SetScene(GameObject* object)
//{
//	object->m_pScene = this;
//
//	for (auto child : object->m_Children)
//	{
//		SetScene(child);
//	}
//}
