#pragma once
#include <memory>
#include <string>
#include "Dictionary.h"

class GameObject;

class ComponentBase
{

	friend class GameObject;

protected:

	ComponentBase() = default;

public:

	virtual ~ComponentBase() = default;

	ComponentBase(const ComponentBase& other)				= delete;
	ComponentBase(ComponentBase&& other)					= delete;
	ComponentBase& operator=(const ComponentBase& other)	= delete;
	ComponentBase& operator=(ComponentBase&& other)			= delete;

private:

	/** Updates the component.*/
	virtual void Update(float) {}

	/** Renders the component.*/
	virtual void Render() const {}

	/** Renders the ImGui in debug mode*/
	virtual void RenderImGui() {}

	/** Gets called after all components are initialized.*/
	virtual void BeginPlay() {}

	//TODO there might be a better way to do this with typeIds
	/** Gets the name of the component for debugging purposes*/
	virtual const std::string GetComponentName() = 0;

public:

	/**
	 * Returns a dictionary of the class defaults so it may be used for file initialization
	 * key -> value
	 */
	virtual Dictionary& GetClassDefault() = 0;

	/**
	 * Initialize the components given the dictionary that contains the value for each parameter given in the GetClassDefault() method
	 */
	virtual void InitializeComponent(const Dictionary& dictionary) = 0;

public:

	/** Returns weak reference of this component that will be invalidated once this component goes out of scope*/
	std::weak_ptr<GameObject> GetWeakReference() { return m_Reference; }

	/** Returns the parent of this component*/
	GameObject* GetParent() { return m_pParent; }

private:

	/** Sets the parent of this object. gets called by the GameObject it gets added to*/
	void SetParent(GameObject* pParent) { m_pParent = pParent; }

private:

	GameObject* m_pParent{};

	/** Shared pointer with empty destructor for which you can ask a weak reference to*/
	std::shared_ptr<GameObject> m_Reference;

};