#include "EngineFiles/GameObject.h"
#include "EngineFiles/Scene.h"
#include "EngineFiles/ComponentBase.h"
#include "Components/Transform.h"
#include "Components/RenderComponent.h"
#include "imgui.h"
#include "Singletons/GUIManager.h"

GameObject::GameObject()
	: m_Reference	{ std::shared_ptr<GameObject>(this,[](GameObject*){}) }
{
	m_pTransform = AddComponent<Transform>();
}

GameObject::~GameObject()
{
	for (auto comp : m_Components)
	{
		delete comp.second;
	}

	for (auto obj : m_Children)
	{
		delete obj;
	}

	// signal scene that it has been destroyed
	//if (m_pScene) m_pScene->RemoveObject(this); //TODO fix this so it doesnt invalidate the for loop when deleting the scene
}

void GameObject::Update(float deltaTime)
{
	for (auto comp : m_Components)
	{
		comp.second->Update(deltaTime);
	}

	for (auto obj : m_Children)
	{
		obj->Update(deltaTime);
	}
}

void GameObject::LateUpdate()
{
	for (auto comp : m_Components)
	{
		comp.second->LateUpdate();
	}

	for (auto obj : m_Children)
	{
		obj->LateUpdate();
	}
}

void GameObject::Render() const
{
	if (m_pRenderComponent) m_pRenderComponent->Render();

	for (auto obj : m_Children)
	{
		obj->Render();
	}
}

void GameObject::BeginPlay()
{
	for (auto comp : m_Components)
	{
		comp.second->BeginPlay();
	}
	m_HasBegunPlay = true;

	for (auto obj : m_Children)
	{
		if (!obj->m_HasBegunPlay)obj->BeginPlay();
	}
}

void GameObject::InitializeComponents()
{
	for (auto comp : m_Components)
	{
		comp.second->Initialize();
	}
	m_HasBeenInitialized = true;

	for (auto obj : m_Children)
	{
		if (!obj->m_HasBeenInitialized)obj->InitializeComponents();
	}
}

void GameObject::RenderImGui()
{
	//if (ImGui::TreeNode("Components")) {
	//
	//	for (auto comp : m_Components)
	//	{
	//		ImGui::PushID(comp.second);
	//		if (ImGui::TreeNodeEx(comp.second->GetComponentName().c_str(), ImGuiTreeNodeFlags_Leaf)) {
	//			if (ImGui::IsItemClicked())
	//				RENDER.SetSelectedComponent(comp.second);
	//			ImGui::TreePop();
	//		}
	//		ImGui::PopID();
	//	}
	//	ImGui::TreePop();
	//}

	//if (ImGui::TreeNodeEx("Children", ImGuiTreeNodeFlags_Leaf * m_Children.empty())) {
	//ImGuiPopup();

	ImGui::PushID(this);

	bool treeNode = (ImGui::TreeNodeEx(GetName().empty() ?
		std::string("GameObject" + std::to_string(GetId())).c_str() :
		GetName().c_str(),
		ImGuiTreeNodeFlags_Leaf * m_Children.empty()));

	ImGuiPopup();
	if (ImGui::IsItemClicked())
		GUI.SetSelectedObject(this);

	if (treeNode)
	{
		for (auto& child : m_Children)
		{
			child->RenderImGui();
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void GameObject::Destroy()
{
	m_pScene->DestroyObject(this);
}

std::weak_ptr<GameObject> GameObject::GetWeakReference() const
{
	return m_Reference;
}

ComponentBase* GameObject::GetComponentById(uint32_t typeId)
{
	auto it = m_Components.find(typeId);
	if (it != m_Components.end())
	{
		return it->second;
	}
	return nullptr;
}

void GameObject::RemoveComponent(ComponentBase* pComponent)
{
	if (!pComponent) return;

	for (auto it = m_Components.begin(); it != m_Components.end(); ++it)
	{
		if (it->second == pComponent) {
			m_Components.erase(it);
			delete pComponent;
			break;
		}
	}
}

void GameObject::SetParent(GameObject* pObject)
{
	// Check if already has a parent and break link between those
	if (m_Parent)
	{
		m_Parent->m_Children.SwapRemove(this);
	}

	// Set as parent
	m_Parent = pObject;

	if (pObject) 
	{
		// Set as child
		pObject->m_Children.emplace_back(this);

		// Set the scene
		if (m_pScene != pObject->m_pScene)
		SetScene(pObject->m_pScene);

		// initialize game object
		if (pObject->m_HasBeenInitialized) InitializeComponents();
		if (pObject->m_HasBegunPlay) BeginPlay();
	}
	else if (m_pScene)
	{
		m_pScene->UnregisterObject(this);
	}

	// Set transform relative to parent
	m_pTransform->Move({});
}

void GameObject::Serialize(std::ostream& os)
{
	os << m_ObjectId << ' ' << m_Name << "\n";

	for (auto& component : m_Components)
	{
		os << component.second->GetComponentName() << '\n' << "{\n";
		component.second->Serialize(os);
		os << "}\n";
	}

	os << "{\n";

	for (auto child : m_Children)
	{
		child->Serialize(os);
	}

	os << "}\n";
}

void GameObject::Deserialize(Deserializer& is)
{
	unsigned int streamId{};
	*is.GetStream() >> streamId;
	is.RegisterGameObject(streamId, this);

	std::getline(*is.GetStream(), m_Name);
	TrimWhitespace(m_Name);

	auto& types = TypeInformation::GetInstance();

	while (is.PeekChar() == 'c')
	{
		// get the class name from the stream
		std::string componentName;
		std::getline(*is.GetStream(), componentName);

		// get the class id
		auto typeInfo = types.GetTypeInfo(componentName);
		assert(typeInfo);

		// generate and add the component
		auto pComponent = typeInfo->componentGenerator(this);

		// deserialize the component
		pComponent->Deserialize(is);
	}

	if (is.CanContinue())
	{
		while (!is.IsEnd())
		{
			auto go = new GameObject();
			go->SetParent(this);
			go->Deserialize(is);
		}
	}
}

void GameObject::SetName(const std::string& name)
{
	m_Name = name;
}

std::string GameObject::GetDisplayName() const
{
	return m_Name.empty() ?
		(std::string("GameObject") + std::to_string(m_ObjectId)).c_str() :
		m_Name.c_str();
}

void GameObject::SetScene(Scene* pScene)
{
	if (pScene != m_pScene)
	{
		if (m_pScene)
			m_pScene->UnregisterObject(this);
		if (pScene)
			pScene->RegisterObject(this);
	}

	m_pScene = pScene;

	for (auto child : m_Children)
	{
		child->SetScene(pScene);
	}
}

void GameObject::Copy(GameObject* originalObject)
{
	for (auto& comp : originalObject->m_Components)
	{
		comp.second->MakeCopy(this);
	}
}

void GameObject::ImGuiPopup()
{
	bool changeName{};
	bool deleteObj{};
	static char buffer[32]{};

	//ImGui::OpenPopup("Object settings");

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete Object"))
		{
			deleteObj = true;
		}

		if (ImGui::MenuItem("Change Name"))
		{
			changeName = true;
			std::memcpy(buffer, m_Name.c_str(), m_Name.size());
		}

		if (ImGui::MenuItem("Duplicate"))
		{
			auto go = new GameObject();
			go->Copy(this);
			if (m_Parent)
				go->SetParent(m_Parent);
			else
				m_pScene->Add(go);
			go->SetName(m_Name);
			go->SetTag(m_Tag);
		}

		ImGui::EndPopup();
	}

	if (changeName)
		ImGui::OpenPopup("Change Obj Name");

	if (deleteObj)
		ImGui::OpenPopup("Delete Object");

	if (ImGui::BeginPopupModal("Change Obj Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("new name", buffer, 32);
		if (ImGui::Button("Change Name"))
		{
			SetName(std::string(buffer));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Delete Object", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Are you sure you want to delete the object?");
		if (ImGui::Button("Delete"))
		{
			Destroy();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
