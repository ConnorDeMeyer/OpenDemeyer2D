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
		delete comp;
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
		comp->Update(deltaTime);
	}

	for (auto obj : m_Children)
	{
		obj->Update(deltaTime);
	}
}

void GameObject::Render() const
{
	for (auto comp : m_Components)
	{
		comp->Render();
	}

	for (auto obj : m_Children)
	{
		obj->Render();
	}
}

void GameObject::BeginPlay()
{
	for (auto comp : m_Components)
	{
		comp->BeginPlay();
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
			if (ImGui::TreeNode(comp->GetComponentName().c_str())) {
				comp->RenderImGui();
				ImGui::TreePop();
			}
			++counter;
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Children", ImGuiTreeNodeFlags_Leaf * m_Children.empty())) {
		for (auto obj : m_Children)
		{
			obj->RenderImGui();
		}
		ImGui::TreePop();
	}
}

void GameObject::Destroy()
{
	m_pScene->DestroyObject(this);
}

std::weak_ptr<GameObject> GameObject::GetWeakReference()
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

	m_Components.SwapRemove(pComponent);
}

void GameObject::AttachGameObject(GameObject* object)
{
	if (!object) return;

	// Check the parents of object for a reference to this object.
	// will not attach if it encounter it to avoid an infinite loop 

	GameObject* parent = object;
	while (parent)
	{
		if (parent == this) return;
		parent = parent->m_Parent;
	}

	if (object->m_Parent) object->m_Parent->RemoveChild(object);
	m_Children.push_back(object);
	object->m_Parent = this;

	if (m_HasBeenInitialized) object->BeginPlay();
}

void GameObject::RemoveChild(GameObject* pObject)
{
	if (!pObject) return;

	m_Children.SwapRemove(pObject);
	pObject->m_Parent = nullptr;
}

void GameObject::BinaryDataOut(std::ostream&)
{
	// Start by sending the information about the components
	
}

void GameObject::BinaryDataIn(std::istream&)
{

}
