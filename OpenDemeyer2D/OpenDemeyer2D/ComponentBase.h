#pragma once
#include <memory>
#include <string>
#include "Dictionary.h"
#include <unordered_map>
#include <typeindex>
#include <functional>
#include "Singleton.h"

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

	//TODO there might be a better way to do this with typeIds
	/** Gets the name of the component for debugging purposes*/
	virtual const std::string GetComponentName() { return "Component???"; }

public:

	/**
	 * Returns a dictionary of the class defaults so it may be used for file initialization
	 * key -> value
	 */
	//virtual Dictionary& GetClassDefault() = 0;

	/**
	 * Initialize the components given the dictionary that contains the value for each parameter given in the GetClassDefault() method
	 */
	//virtual void InitializeComponent(const Dictionary& dictionary) = 0;

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

//static std::unordered_map<std::type_index, std::function<ComponentBase* ()>> ClassGenerator{};
//static std::unordered_map<std::string, std::type_index> ClassNameIds{};

class TypeInformation : public Singleton<TypeInformation>
{
public:
	std::unordered_map<std::type_index, std::function<ComponentBase* ()>> ClassGenerator;
	std::unordered_map<std::string, std::type_index> ClassNameIds;
};

template <typename T>
class TypeIdentifier final
{
public:

	TypeIdentifier()
	{
		TypeInformation::GetInstance().ClassNameIds.insert({ std::string(typeid(T).name()), std::type_index(typeid(T)) });
		TypeInformation::GetInstance().ClassGenerator.insert({ std::type_index(typeid(T)), [] {return new T(); } });
	}

};


template <typename T>
class Component : public ComponentBase
{
	const std::string GetComponentName() override { return typeid(T).name(); }

private:

	inline static TypeIdentifier<T> TypeIdentifier{};
};