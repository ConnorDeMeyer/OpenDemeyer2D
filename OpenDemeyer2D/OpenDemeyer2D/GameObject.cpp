#include "OD2pch.h"
#include "GameObject.h"
#include "Scene.h"
#include "ComponentBase.h"
#include "Components/Transform.h"
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
			if (ImGui::TreeNode(std::string("GameObject" + std::to_string(i)).c_str())) {
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
		m_pScene = pObject->m_pScene;

		// initialize game object
		if (pObject->m_HasBeenInitialized) BeginPlay();
	}

	// Set transform relative to parent
	m_pTransform->Move({});
}

//void GameObject::AttachGameObject(GameObject* object)
//{
//	if (!object) return;
//
//	// Check the parents of object for a reference to this object.
//	// will not attach if it encounter it to avoid an infinite loop 
//
//	GameObject* parent = object;
//	while (parent)
//	{
//		if (parent == this) return;
//		parent = parent->m_Parent;
//	}
//
//	if (object->m_Parent) object->m_Parent->RemoveChild(object);
//	m_Children.emplace_back(object);
//	object->m_Parent = this;
//	object->m_pScene = m_pScene;
//
//	if (m_HasBeenInitialized) object->BeginPlay();
//}
//
//void GameObject::RemoveChild(GameObject* pObject)
//{
//	if (!pObject) return;
//
//	m_Children.SwapRemove(pObject);
//	pObject->m_Parent = nullptr;
//}
