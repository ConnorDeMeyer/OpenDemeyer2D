#pragma once

#include <unordered_map>
#include <typeindex>
#include "UtilityFiles/ODArray.h"
#include <memory>
#include <cassert>

#include "EngineIO/Deserializer.h"

class Scene;
class Transform;
class RenderComponent;

/**
* Class for helping linking fields that need a reference to the copied game object when being copied
*/
class CopyLinker final
{
	struct LinkingAction
	{
		GameObject* originalObject{};
		std::function<void(GameObject*)> Action;
	};

public:
	void LinkWithNewObject(GameObject* originalObject, const std::function<void(GameObject*)>& linkingAction);
	void RegisterObject(GameObject* originalObject, GameObject* newObject);
	/**Only call at the end of copying*/
	void PerformLinkingActions();
private:
	std::unordered_map<GameObject*, GameObject*> m_RegisteredObjects;
	std::vector<LinkingAction> m_LinkingActions;
};

/** 
* This class is a container for components and is responsible for updating, rendering and managing them.
* It is part of the scene tree and is capable of having child Game objects who will maintain relative location to its parent.
*/
class GameObject final
{

	friend class Scene;

public:

	GameObject();

	/** 
	* Will destroy the game object immediately.
	* Use the destroy method to destroy it at the end of the frame
	*/
	~GameObject();

	GameObject(const GameObject& other)				= delete;
	GameObject(GameObject&& other)					= delete;
	GameObject& operator=(const GameObject& other)	= delete;
	GameObject& operator=(GameObject&& other)		= delete;

public:

	/** Updates the components attached to this component.*/
	void Update(float deltaTime);

	/** Update the components after the normal update*/
	void LateUpdate();

	/** Calls the BeginPlay method of the components after all components have begun playing.*/
	void BeginPlay();

	/** Renders the components attached to this component.*/
	void Render() const;

	/** Initialized all the components*/
	void InitializeComponents();

	/** Will flag the Game Object for deletion at the end of the frame*/
	void Destroy();

	/** Returns weak reference of this object that will be invalidated once this object goes out of scope*/
	std::weak_ptr<GameObject> GetWeakReference() const;

	/** Get the parent game object or nullptr if not attached to anything*/
	GameObject* GetParent() const { return m_Parent; }

	/** 
	* Returns the component that is of type T.
	* Uses the type Id to find the component.
	*/
	template <typename T>
	T* GetComponent() const;

	/**
	* Returns the component that is of type T.
	* Uses dynamic_casts to find the right object.
	* May also return an inhereted subclass.
	*/
	template <typename T>
	T* GetComponentByCast() const;

	/**
	* Returns the component whose typeid matches that of the given type id
	* Returns a ComponentBase* of the right type, You may have to cast it into the type of the component
	* Return nullptr if none was found
	*/
	ComponentBase* GetComponentById(uint32_t typeId);

	/**
	* Adds a component to the object
	* Gives the engine the ability to manage the component and makes sure the component gets automatically deleted
	*/
	template<typename  T>
	T* AddComponent();

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

	void SetParent(Scene& scene);

	void AddChild(GameObject* pObject);

	/**
	* Get the scene it is currently in.
	* Returns nullptr if it is not in a scene.
	*/
	Scene* GetScene() const { return m_pScene; }

	/** Get the underlying unordered map that containts the components. */
	inline const std::unordered_map<uint32_t, ComponentBase*>& GetComponents() { return m_Components; };

	/** Serialize this game object into a stream*/
	void Serialize(std::ostream& os);

	void Deserialize(Deserializer& is);

	/** 
	* Set the tag of this object.
	* The tag may be gotten by using the GetTag method
	*/
	void SetTag(const std::string& tag) { m_Tag = tag; }

	/**
	* Returns the tag of this object.
	* Can be used to compare objects at runtime without having to check for components
	*/
	const std::string& GetTag() const { return m_Tag; }

	/** Returns the scene id of the object*/
	unsigned int GetId() const { return m_ObjectId; }

	void SetName(const std::string& name);

	const std::string& GetName() const { return m_Name; }

	std::string GetDisplayName() const;

	void SetScene(Scene* pScene);

	void Copy(GameObject* originalObject, CopyLinker* copyLinker = nullptr);

private:

	/** Container of all the components attached to this GameObject.*/
	//ODArray<ComponentBase*> m_Components;

	std::unordered_map<uint32_t, ComponentBase*> m_Components;

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

	uint32_t m_ObjectId{};

	std::string m_Tag{};

	std::string m_Name{};

	bool m_HasBeenInitialized{};
	bool m_HasBegunPlay{};
};


/**
 * TEMPLATE METHODS
 */

template<typename T>
T* GameObject::GetComponent() const
{
	if constexpr (std::is_same_v<T, Transform>) {return reinterpret_cast<T*>(m_pTransform);}
	else if constexpr (std::is_same_v<T, RenderComponent>) {return reinterpret_cast<T*>(m_pRenderComponent);}
	else {
		constexpr uint32_t typeId{ class_id<T>() };
		auto it = m_Components.find(typeId);
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
T* GameObject::AddComponent()
{
	if constexpr (std::is_same_v<T, Transform>)
	{
		if (m_pTransform)
			return m_pTransform;
	}

	// Make sure the component is not already added
	if (GetComponent<T>())
	{
		throw std::runtime_error("Component already in object");
	}
	T* comp = new T();
	constexpr uint32_t typeId{ class_id<T>() };
	m_Components.insert({ typeId, comp});

	comp->m_pParent = this;

	if (m_HasBeenInitialized) comp->Initialize();
	if (m_HasBegunPlay) comp->BeginPlay();

	if constexpr (std::is_same_v<T, RenderComponent>)
	{
		m_pRenderComponent = reinterpret_cast<RenderComponent*>(comp);
	}

	return comp;
}

template<typename T>
void GameObject::RemoveComponent()
{
	if constexpr (std::is_same_v<T, Transform>) { throw std::runtime_error("Cannot remove transform component"); }

	constexpr uint32_t typeId{ class_id<T>() };
	auto it = m_Components.find(typeId);
	if (it != m_Components.end())
	{
		delete it->second;
		m_Components.erase(it);
	}
}