#include "OD2pch.h"
#include "GameObject.h"
#include "Scene.h"
#include "ComponentBase.h"
#include "Components/Transform.h"
#include "Components/RenderComponent.h"
#include "imgui.h"

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
	m_HasBeenInitialized = true;

	for (auto obj : m_Children)
	{
		if (!obj->m_HasBeenInitialized)obj->BeginPlay();
	}
}

void GameObject::RenderImGui()
{
	if (ImGui::TreeNode("Components")) {
		size_t counter{};
		for (auto comp : m_Components)
		{
			if (ImGui::TreeNode(comp.second->GetComponentName().c_str())) {
				comp.second->RenderImGui();
				ImGui::TreePop();
			}
			++counter;
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Children", ImGuiTreeNodeFlags_Leaf * m_Children.empty())) {
		for (size_t i{}; i < m_Children.size(); ++i)
		{
			if (ImGui::TreeNode(std::string("GameObject" + std::to_string(m_Children[i]->GetId())).c_str())) {
				m_Children[i]->RenderImGui();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}

void GameObject::Destroy()
{
	m_pScene->DestroyObject(this);
}

std::weak_ptr<GameObject> GameObject::GetWeakReference() const
{
	return m_Reference;
}

ComponentBase* GameObject::GetComponentById(std::type_index typeId)
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

	/*size_t counter{};
	for (ComponentBase* component : m_Components) {
		if (pComponent == component)
		{
			delete component;
			m_Components.SwapRemove(counter);
			break;
		}
		else ++counter;
	}*/

	//m_Components.SwapRemove(pComponent);
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

	if (pObject) {

		// Set as child
		pObject->m_Children.emplace_back(this);

		// Set the scene
		if (m_pScene != pObject->m_pScene)
		SetScene(pObject->m_pScene);

		// initialize game object
		if (pObject->m_HasBeenInitialized) BeginPlay();
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
	os << m_ObjectId << "\n"; //TODO set object id

	for (auto& component : m_Components)
	{
		os << component.second->GetComponentName() << '\n' << "{\n";
		component.second->Serialize(os);
		os << "},\n";
	}

	os << "{\n";

	for (auto child : m_Children)
	{
		child->Serialize(os);
	}

	os << "}\n";
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
