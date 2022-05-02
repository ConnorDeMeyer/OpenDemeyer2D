#pragma once
#include <memory>
#include <string>
#include "Dictionary.h"
#include "Singleton.h"
#include <unordered_map>
#include <functional>
#include <typeindex>

class GameObject;

class ComponentBase
{

	friend class GameObject;

protected:

	ComponentBase()
		: m_Reference{ std::shared_ptr<ComponentBase>(this,[](ComponentBase*) {}) }
	{}

public:

	virtual ~ComponentBase() = default;

	ComponentBase(const ComponentBase& other) = delete;
	ComponentBase(ComponentBase&& other) = delete;
	ComponentBase& operator=(const ComponentBase& other) = delete;
	ComponentBase& operator=(ComponentBase&& other) = delete;

private:

	/** Updates the component.*/
	virtual void Update(float) {}

	/** Gets called after all object get updated*/
	virtual void LateUpdate() {}

	/** Renders the ImGui in debug mode*/
	virtual void RenderImGui() {}

	/** Gets called after all components are initialized.*/
	virtual void BeginPlay() {}

public:

	/** Gets the name of the component for debugging purposes*/
	virtual const std::string GetComponentName() const = 0;

	virtual std::type_index GetComponentId() const = 0;

	virtual void Serialize(std::ostream& stream) const = 0;

	//virtual std::function<ComponentBase* (GameObject*)> _Gameobject_Add() = 0;

public:

	/** Returns weak reference of this component that will be invalidated once this component goes out of scope*/
	std::weak_ptr<ComponentBase> GetWeakReference() const { return m_Reference; }

	/** Returns the parent of this component*/
	GameObject* GetParent() const { return m_pParent; }

private:

	/** Sets the parent of this object. gets called by the GameObject it gets added to*/
	void SetParent(GameObject* pParent) { m_pParent = pParent; }

private:

	GameObject* m_pParent{};

	/** Shared pointer with empty destructor for which you can ask a weak reference to*/
	std::shared_ptr<ComponentBase> m_Reference;

};