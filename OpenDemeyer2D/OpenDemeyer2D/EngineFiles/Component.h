#pragma once
#include <memory>
#include <string>
#include "UtilityFiles/Dictionary.h"
#include "UtilityFiles/Singleton.h"
#include <unordered_map>
#include <functional>
#include <typeindex>

#include "EngineFiles/ComponentBase.h"
#include "EngineIO/TypeInformation.h"
#include "EngineIO/Deserializer.h"
#include "EngineFiles/GameObject.h"
#include "EngineIO/Reflection.h"


template <typename T>
class Component : public ComponentBase
{

	friend class TypeIdentifier<T>;

public:

	constexpr const std::string_view GetComponentName() const override { return type_name<T>(); }

	constexpr uint32_t GetComponentId() const override { return hash(type_name<T>()); }

	virtual void DefineUserFields(UserFieldBinder&) const {};

	void Serialize(std::ostream& os) const override
	{
		constexpr uint32_t id{ class_id<T>() };
		auto typeInfo = TypeInformation::GetInstance().GetTypeInfo(id);
		assert(typeInfo);

		for (auto& field : typeInfo->field.GetFields())
		{
			os << field.first << ' '; 
			field.second.Serialize(os, this);
			os << '\n';
		}
	}

	void Deserialize(Deserializer& is) override
	{
		constexpr uint32_t id{ class_id<T>() };
		auto typeInfo = TypeInformation::GetInstance().GetTypeInfo(id);
		assert(typeInfo);

		if (CanContinue(*is.GetStream()))
		{
			while (!IsEnd(*is.GetStream()))
			{
				std::string fieldName;
				*is.GetStream() >> fieldName;
				auto it = typeInfo->field.GetFields().find(fieldName);
				if (it != typeInfo->field.GetFields().end())
				{
					it->second.Deserialize(is, this);
				}
			}
		}
	}

	void Copy(const ComponentBase* pOriginal) override
	{
		constexpr uint32_t typeId{ class_id<T>() };
		auto typeInfo = TypeInformation::GetInstance().GetTypeInfo(typeId);

		if (typeInfo)
		{
			for (auto field : typeInfo->field.GetFields())
			{
				field.second.Copy(pOriginal, this);
			}
		}
	}

	ComponentBase* MakeCopy(GameObject* objectToAddto) const override
	{
		ComponentBase* comp{};
		if constexpr (std::is_same_v<Transform, T>)
			comp = objectToAddto->GetTransform();
		else
			comp = objectToAddto->AddComponent<T>();
		comp->Copy(this);
		return comp;
	}

	std::weak_ptr<T> GetWeakReferenceType()
	{
		return std::reinterpret_pointer_cast<T>(m_Reference);
	}

private:

	// Generate type information
	inline static TypeIdentifier<T> TypeIdentifier{};
};
