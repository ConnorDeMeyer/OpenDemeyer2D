#include "OD2pch.h"
#include "Scene.h"

#include "GameObject.h"
#include "imgui.h"
#include "Components/PhysicsComponent.h"

#include <box2d.h>

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

void Scene::Add(GameObject* pObject)
{
	if (pObject->m_pScene) throw ObjectAlreadyInASceneException();

	m_SceneTree.emplace_back(pObject);
	SetScene(pObject);
	pObject->BeginPlay();
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
	for (GameObject* child : m_SceneTree)
	{
		child->Update(deltaTime);
	}

	for (GameObject* object : m_DestroyableObjects)
	{
		object->SetParent(nullptr);
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

void Scene::RenderImGui()
{
	if (ImGui::TreeNode(m_Name.c_str())) {

		for (size_t i{}; i < m_SceneTree.size(); ++i)
		{
			if (ImGui::TreeNode(std::string("GameObject" + std::to_string(i)).c_str())) {
				m_SceneTree[i]->RenderImGui();
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
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

void Scene::SetScene(GameObject* object)
{
	object->m_pScene = this;
	for (auto child : object->m_Children)
	{
		SetScene(child);
	}
}
