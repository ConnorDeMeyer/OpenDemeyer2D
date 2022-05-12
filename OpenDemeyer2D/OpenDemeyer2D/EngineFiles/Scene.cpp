#include "EngineFiles/Scene.h"

#include "EngineFiles/GameObject.h"
#include "imgui.h"
#include "Components/PhysicsComponent.h"

#include <box2d.h>
#include "Singletons/GUIManager.h"

Scene::Scene(const std::string& name)
	: m_Name{ name }
{
	m_pb2World = new b2World(b2Vec2{0, 0});
	m_pb2World->SetContactListener(this);

}

Scene::~Scene()
{
	for (size_t i{}; i < m_SceneTree.size(); ++i)
	{
		delete m_SceneTree[i];
	}

	delete m_pb2World;
}

GameObject* Scene::Add(GameObject* pObject)
{
	if (pObject->m_pScene) throw ObjectAlreadyInASceneException();

	m_UninitializedObject.emplace_back(pObject);

	return pObject;
}

void Scene::DestroyObject(GameObject* pObject)
{
	m_DestroyableObjects.emplace_back(pObject);
}

void Scene::DestroyObjectImmediately(GameObject* pObject)
{
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
		m_SceneTree.emplace_back(pObject);
		pObject->SetScene(this);
		m_NotBegunObjects.emplace_back(pObject);
		pObject->InitializeComponents();
	}
	m_UninitializedObject.clear();

	if (IsPlaying)
	{
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
		object->SetParent(static_cast<GameObject*>(nullptr));
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
			auto go = Add(new GameObject());
			go->Deserialize(is);
		}
	}
}

void Scene::Copy(Scene* originalScene)
{
	m_SceneTree.reserve(originalScene->m_SceneTree.size());

	for (auto pObject : originalScene->m_SceneTree)
	{
		auto go = new GameObject();
		go->Copy(pObject);
		Add(go);
	}
}

void Scene::AddToSceneTree(GameObject* go)
{
	for (auto sgo : m_SceneTree)
	{
		if (sgo == go)
			return;
	}

	m_SceneTree.push_back(go);
}

void Scene::RegisterObject(GameObject* pObject)
{
	uint32_t id = uint32_t(m_RegisteredObjects.size()) + 1;
	pObject->m_ObjectId = id;
	m_RegisteredObjects.insert({ id, pObject });
}

void Scene::UnregisterObject(GameObject* pObject)
{
	m_RegisteredObjects.erase(pObject->m_ObjectId);
	pObject->m_ObjectId = 0;
}

void Scene::BeginContact(b2Contact* contact)
{
	PhysicsComponent* compA = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureA()->GetUserData().pointer);
	PhysicsComponent* compB = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureB()->GetUserData().pointer);

	if (compA && compB) {
		compA->OnOverlap.BroadCast(compB);
		compB->OnOverlap.BroadCast(compA);
	}
}

void Scene::EndContact(b2Contact* contact)
{
	PhysicsComponent* compA = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureA()->GetUserData().pointer);
	PhysicsComponent* compB = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureB()->GetUserData().pointer);

	if (compA && compB) {
		compA->OnEndOverlap.BroadCast(compB);
		compB->OnEndOverlap.BroadCast(compA);
	}
}

void Scene::PreSolve(b2Contact*, const b2Manifold*)
{

}

void Scene::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	PhysicsComponent* compA = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureA()->GetUserData().pointer);
	PhysicsComponent* compB = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureB()->GetUserData().pointer);

	if (compA && compB) {
		compA->OnHit.BroadCast(compB,
			reinterpret_cast<const glm::vec2&>(impulse->normalImpulses),
			reinterpret_cast<const glm::vec2&>(impulse->tangentImpulses));
	}
}

void Scene::RemoveObject(GameObject* object)
{
	m_SceneTree.SwapRemove(object);
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
