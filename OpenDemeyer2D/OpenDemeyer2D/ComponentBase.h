#pragma once
#include <memory>

class GameObject;

class ComponentBase
{

	friend class GameObject;

public:

	ComponentBase() = default;

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

	/** gets called after all components are initialized.*/
	virtual void BeginPlay() {}

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
