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

	RegisterObject(pObject);

	m_UninitializedObject.emplace_back(pObject);
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

	// initialized objects
	for (GameObject* pObject : m_UninitializedObject)
	{
		m_SceneTree.emplace_back(pObject);
		pObject->SetScene(this);
		pObject->BeginPlay();
	}
	m_UninitializedObject.clear();

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

	// Delete the destroyed Objects
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

void Scene::ImGuiScenePopup()
{
	if (ImGui::BeginPopupContextItem("Scene settings"))
	{
		if (ImGui::MenuItem("Delete Scene"))
		{

		}

		if (ImGui::Button("Change Name"))
		{
			ImGui::OpenPopup("Change Scene Name");
		}

		if (ImGui::BeginPopupModal("Change Scene Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			char buffer[32]{};
			ImGui::InputText("new name", buffer, 32);
			if (ImGui::Button("Change Name"))
			{
				Scene::ChangeName(std::string(buffer));
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::MenuItem("Set as active"))
		{
			SCENES.SetActiveScene(this);
		}

		ImGui::EndPopup();
	}
}

void Scene::RenderImGui()
{
	if (ImGui::TreeNode(m_Name.c_str())) {

		ImGuiScenePopup();

		for (size_t i{}; i < m_SceneTree.size(); ++i)
		{
			if (ImGui::TreeNode(std::string("GameObject" + std::to_string(m_SceneTree[i]->GetId())).c_str())) {
				m_SceneTree[i]->RenderImGui();
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	else ImGuiScenePopup();
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

void Scene::Serialize(std::ostream& os)
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

void Scene::RegisterObject(GameObject* pObject)
{
	UINT32 id = UINT32(m_RegisteredObjects.size()) + 1;
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
