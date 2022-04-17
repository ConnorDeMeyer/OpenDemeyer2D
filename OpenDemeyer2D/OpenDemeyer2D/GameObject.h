#pragma once

#include <unordered_map>
#include <typeindex>
#include "ODArray.h"
#include <memory>

#include "ComponentBase.h"
#include "Components/Transform.h"
#include "Components/RenderComponent.h"

class Scene;
class Transform;

/** This class is a container for components and is responsible for updating, rendering and managing them.
 * It is part of the scene tree and is capable of having child Game objects who will maintain relative location to its parent.
 */
class GameObject final
{

	friend class Scene;

public:

	GameObject();

	/** Will destroy the game object immediately.
	 * Use the destroy method to destroy it at the end of the frame
	 */
	~GameObject();

	GameObject(const GameObject& other)				= delete;
	GameObject(GameObject&& other)					= delete;
	GameObject& operator=(const GameObject& other)	= delete;
	GameObject& operator=(GameObject&& other)		= delete;

private:

	/** Updates the components attached to this component.*/
	void Update(float deltaTime);

	/** Update the components after the normal update*/
	void LateUpdate();

	/** Renders the components attached to this component.*/
	void Render() const;

	/** Calls the BeginPlay method of the components after all components are initialized.*/
	void BeginPlay();

	/** Calls the RenderImGui() method on the components*/
	void RenderImGui();

public:

	/** Will flag the Game Object for deletion at the end of the frame*/
	void Destroy();

	/** Returns weak reference of this object that will be invalidated once this object goes out of scope*/
	std::weak_ptr<GameObject> GetWeakReference() const;

	/** Get the parent game object or nullptr if not attached to anything*/
	GameObject* GetParent() const { return m_Parent; }

	/** Returns the first component that is of type T.
		* This method is slow, you should not use it in time critical situations.*/
	template <typename T>
	T* GetComponent() const;

	template <typename T>
	T* GetComponentByCast() const;

	/**
	* Adds a component to the object
	* Gives the engine the ability to manage the component and makes sure the component gets automatically deleted
	*/
	template<typename  T>
	std::enable_if_t<std::is_base_of_v<ComponentBase, T>, T*>
	AddComponent();

	/** Removes the component from the object.
	* The component will be removed at the end of the frame.*/
	void RemoveComponent(ComponentBase* pComponent);

	/** Removes the first component of type T from the object.
	* The component will be removed at the end of the frame.
	* Returns true if an object was removed.*/
	template <typename T>
	void RemoveComponent();

	/** returns the Transform component associated with this object.*/
	Transform* GetTransform() const { return m_pTransform; }

	RenderComponent* GetRenderComponent() const { return m_pRenderComponent; }

	/** Returns a list of all children*/
	const ODArray<GameObject*>& GetChildren() const { return m_Children; }

	/**
	 * Parents the game object to pObject.
	 * @param pObject parent, can be nullptr to parent to the scene.
	 */
	void SetParent(GameObject* pObject);

	Scene* GetScene() const { return m_pScene; }

	/** Attach a Game Object to this Game Object*/
	//void AttachGameObject(GameObject* pObject);

	/** Removes the object from the child list*/
	//void RemoveChild(GameObject* pObject);

private:

	/** Container of all the components attached to this GameObject.*/
	//ODArray<ComponentBase*> m_Components;

	std::unordered_map<std::type_index, ComponentBase*> m_Components;

	/** Container of children attached to this GameObject.*/
	ODArray<GameObject*> m_Children;

	/** Reference to the parent object of this Game Object.
	 * Is nullptr if it has no parent.*/
	GameObject* m_Parent{};

	/** Reference to the owning scene.*/
	Scene* m_pScene{};

	/** Shared pointer with empty destructor for which you can ask a weak reference to*/
	std::shared_ptr<GameObject> m_Reference;

	Transform* m_pTransform{};

	RenderComponent* m_pRenderComponent{};

	bool m_HasBeenInitialized{};
};


/**
 * TEMPLATE METHODS
 */

template<typename T>
T* GameObject::GetComponent() const
{
	if constexpr (std::is_same_v<T, Transform>) {return reinterpret_cast<T*>(m_pTransform);}
	else if constexpr (std::is_same_v<T, Transform>) {return reinterpret_cast<T*>(m_pRenderComponent);}
	else {
		auto it = m_Components.find(typeid(T));
		return (it != m_Components.end()) ? reinterpret_cast<T*>(it->second) : nullptr;
	}
}

template <typename T>
T* GameObject::GetComponentByCast() const
{
	for (auto component : m_Components)
		if (auto returnValue{ dynamic_cast<T*>(component.second) }) return returnValue;
	return nullptr;
}

template <typename T>
std::enable_if_t<std::is_base_of_v<ComponentBase, T>, T*>
GameObject::AddComponent()
{
	if constexpr (std::is_same_v<T, RenderComponent>)
	{
		if (m_pRenderComponent) throw std::runtime_error("Component already in gameobject");

		m_pRenderComponent = new RenderComponent();
		m_Components.insert({ typeid(T), m_pRenderComponent });
		m_pRenderComponent->m_pParent = this;

		if (m_HasBeenInitialized) m_pRenderComponent->BeginPlay();

		return m_pRenderComponent;
	}
	else
	{
		assert(!GetComponent<T>()); // Make sure the component is not already added
		T* comp = new T();
		m_Components.insert({ typeid(T), comp });

		comp->m_pParent = this;

		if (m_HasBeenInitialized) comp->BeginPlay();
		return comp;
	}
}

template<typename T>
void GameObject::RemoveComponent()
{
	if constexpr (std::is_same_v<T, Transform>) { throw std::runtime_error("Cannot remove transform component"); }

	auto it = m_Components.find(typeid(T));
	if (it != m_Components.end())
	{
		delete it->second;
		m_Components.erase(it);
	}
	//size_t counter{};
	//for (ComponentBase* component : m_Components)
	//{
	//	if (dynamic_cast<T*>(component))
	//	{
	//		delete component;
	//		m_Components.SwapRemove(counter);
	//		break;
	//	} else ++counter;
	//}
}