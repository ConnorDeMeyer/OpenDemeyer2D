#include "GameObject.h"
#include "Scene.h"
#include "ComponentBase.h"
#include "Components/Transform.h"

GameObject::GameObject()
	: m_Reference	{ std::shared_ptr<GameObject>(this,[](GameObject*){}) }
	, m_pTransform	{ new Transform() }
{
	AddComponent(m_pTransform);
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
	if (m_pScene) m_pScene->RemoveObject(this);
}

void GameObject::Update(float deltaTime)
{
	for (auto comp : m_Components)
	{
		comp->Update(deltaTime);
	}
}

void GameObject::Render() const
{
	for (auto comp : m_Components)
	{
		comp->Render();
	}
}

void GameObject::BeginPlay()
{
	for (auto comp : m_Components)
	{
		comp->BeginPlay();
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

void GameObject::AddComponent(ComponentBase* pComponent)
{
	if (pComponent) {
		m_Components.push_back(pComponent);
		pComponent->SetParent(this);
	}
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

void GameObject::BinaryDataOut(std::ostream&)
{
	// Start by sending the information about the components
	
}

void GameObject::BinaryDataIn(std::istream&)
{

}
