#pragma once
#include <memory>
#include <string_view>
#include "UtilityFiles/Dictionary.h"
#include <unordered_map>
#include <functional>

#include "EngineIO/Deserializer.h"
#include "EngineIO/TypeInformation.h"
#include "EngineFiles/GameObject.h"
#include "EngineIO/Reflection.h"

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

	/**
	* Gets called right after the constructor fires and after the user fields are initialized
	* Used to initialize and ling components that can not be done in the constructor.
	* Do not create more components or objects in this method, as those created will be saved to the game file
	*/
	virtual void Initialize() {}

	/** 
	* Gets called when the game is started or when this component was created during gameplay.
	*/
	virtual void BeginPlay() {}

public:

	/** Renders the ImGui in debug mode*/
	virtual void RenderImGui() {}

	/** Gets the name of the component for debugging purposes*/
	virtual constexpr const std::string_view GetComponentName() const = 0;

	virtual constexpr uint32_t GetComponentId() const = 0;

	virtual void Serialize(std::ostream& stream) const = 0;

	virtual void Deserialize(Deserializer& is) = 0;

	/** Copy values from the original when getting copied*/
	virtual void Clone(const ComponentBase* pOriginal, CopyLinker* copyLinker = nullptr)
	{
		uint32_t typeId = GetComponentId();
		auto typeInfo = TypeInformation::GetInstance().GetTypeInfo(typeId);

		if (typeInfo)
		{
			for (auto field : typeInfo->field.GetFields())
			{
				field.second.Copy(pOriginal, this, copyLinker);
			}
		}
	}

	virtual ComponentBase* MakeCopy(GameObject* objectToAddto, CopyLinker* copyLinker = nullptr) const = 0;

	virtual void DefineUserFields(UserFieldBinder&) const {};

public:

	/** Returns weak reference of this component that will be invalidated once this component goes out of scope*/
	std::weak_ptr<ComponentBase> GetWeakReference() const { return m_Reference; }

	/** Returns the Gameobject that this component is attached to*/
	inline GameObject* GetObject() const { return m_pParent; }

private:

	/** Sets the parent of this object. gets called by the GameObject it gets added to*/
	void SetParent(GameObject* pParent) { m_pParent = pParent; }

protected:

	inline Transform* GetTransform() const { return GetObject()->GetTransform(); }
	inline RenderComponent* GetRenderComponent() const { return GetObject()->GetRenderComponent(); }
	inline Scene* GetScene() const { return GetObject()->GetScene(); }

	template <typename T>
	inline T* GetComponent() const { return GetObject()->GetComponent<T>(); }

private:

	GameObject* m_pParent{};

protected:

	/** Shared pointer with empty destructor for which you can ask a weak reference to*/
	std::shared_ptr<ComponentBase> m_Reference;

};

/**
* SERIALIZING AND DESERIALIZING weak_ptr containing Components
*/

template <typename T,
	typename = std::enable_if_t<std::is_base_of_v<ComponentBase, T>>>
std::ostream& operator<<(std::ostream& stream, const std::weak_ptr<T> component)
{
	if (component.expired()) return stream << 0;
	return stream << component.lock()->GetObject()->GetId();
}

template <typename T,
	typename = std::enable_if_t<std::is_base_of_v<ComponentBase,T>>>
Deserializer& operator>>(Deserializer& stream, std::weak_ptr<T>* component)
{
	uint32_t id{};
	(*stream.GetStream()) >> id;

	auto linker = [component](std::weak_ptr<ComponentBase>* comp) -> void
	{
		*component = std::reinterpret_pointer_cast<T>(comp->lock());
	};

	constexpr uint32_t typeId{ class_id<T>() };
	stream.GetComponentFromObject(id, typeId, linker);

	return stream;
}

/**
* COPYING weak_ptr containing Components
*/

template <typename T>
void CopyWeakPtr(const std::weak_ptr<T>& original, std::weak_ptr<T>& copy, CopyLinker* copyLinker = nullptr)
{
	if constexpr (std::is_base_of_v<ComponentBase, T>)
	{
		if (copyLinker && !original.expired())
		{
			// copy the same reference if inside the same scene
			if (copyLinker->IsSameScene())
			{
				copy = original;
			}

			// in case the referenced object was also copied we take the reference from the newly copied object instead of the old one
			copyLinker->LinkWithNewObject(original.lock()->GetObject(), [&copy, &original](GameObject* obj)
				{
					copy = std::reinterpret_pointer_cast<T>(obj->GetComponentById(original.lock()->GetComponentId())->GetWeakReference().lock());
				});
		}
	}
	else
	{
		copy = original;
	}
}

/**
* MACROS FOR CREATING COMPONENT TYPE INFORMATION
*/

#define GetComponentNameFuncDef(TypeName) constexpr const std::string_view GetComponentName() const override { return type_name<TypeName>(); }
#define GetComponentIdFuncDef(TypeName) constexpr uint32_t GetComponentId() const override { return hash(type_name<TypeName>()); }
#define SerializeFuncDef(TypeName) void Serialize(std::ostream& os) const override\
{\
	constexpr uint32_t id{ class_id<TypeName>() };\
	auto typeInfo = TypeInformation::GetInstance().GetTypeInfo(id);\
	assert(typeInfo);\
\
	for (auto& field : typeInfo->field.GetFields())\
	{\
		os << field.first << ' ';\
		field.second.Serialize(os, this);\
		os << '\n';\
	}\
}
#define DeserializeFuncDef(TypeName) void Deserialize(Deserializer& is) override\
{\
	constexpr uint32_t id{ class_id<TypeName>() };\
	auto typeInfo = TypeInformation::GetInstance().GetTypeInfo(id);\
	assert(typeInfo);\
\
	if (CanContinue(*is.GetStream()))\
	{\
		while (!IsEnd(*is.GetStream()))\
		{\
			std::string fieldName;\
			*is.GetStream() >> fieldName;\
			auto it = typeInfo->field.GetFields().find(fieldName);\
			if (it != typeInfo->field.GetFields().end())\
			{\
				it->second.Deserialize(is, this);\
			}\
		}\
	}\
}
#define MakeCopyFuncDef(TypeName) ComponentBase* MakeCopy(GameObject* objectToAddto, CopyLinker* copyLinker = nullptr) const override\
{\
	ComponentBase* comp{};\
	comp = objectToAddto->AddComponent<TypeName>();\
	comp->Clone(this, copyLinker);\
	return comp;\
}
#define GetWeakReferenceTypeFuncDef(TypeName) std::weak_ptr<TypeName> GetWeakReferenceType()\
{\
	return std::reinterpret_pointer_cast<TypeName>(m_Reference);\
}

#define COMPONENT_BODY(TypeName)\
public:\
GetComponentNameFuncDef(TypeName)\
GetComponentIdFuncDef(TypeName)\
SerializeFuncDef(TypeName)\
DeserializeFuncDef(TypeName)\
MakeCopyFuncDef(TypeName)\
GetWeakReferenceTypeFuncDef(TypeName)\
private:\
inline static TypeIdentifier<TypeName> TypeIdentifier{};