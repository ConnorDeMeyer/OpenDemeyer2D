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
	bool changeName{};
	static char buffer[32]{};

	if (ImGui::BeginPopupContextItem("Scene settings"))
	{
		if (ImGui::MenuItem("Delete Scene"))
		{
			SCENES.RemoveScene(this);
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Change Name"))
		{
			changeName = true;
			std::memcpy(buffer, m_Name.c_str(), m_Name.size());
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Set as active"))
		{
			SCENES.SetActiveScene(this);
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Add GameObject"))
		{
			Add(new GameObject());
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (changeName)
		ImGui::OpenPopup("Change Scene Name");

	if (ImGui::BeginPopupModal("Change Scene Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("new name", buffer, 32);
		if (ImGui::Button("Change Name"))
		{
			ChangeName(std::string(buffer));
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void Scene::RenderImGui()
{
	ImGui::PushID(this);

	if (ImGui::BeginTabItem(m_Name.c_str())) {

		ImGuiScenePopup();

		for (size_t i{}; i < m_SceneTree.size(); ++i)
		{
			auto pObject{ m_SceneTree[i] };
			pObject->RenderImGui();
		}

		ImGui::EndTabItem();
	}
	else ImGuiScenePopup();

	ImGui::PopID();
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
