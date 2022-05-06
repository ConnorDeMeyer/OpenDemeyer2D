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

	const std::string_view GetComponentName() const override { __FUNCSIG__; return type_name<T>(); }

	uint32_t GetComponentId() const override { return hash(type_name<T>()); }

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

	std::weak_ptr<T> GetWeakReferenceType()
	{
		return std::reinterpret_pointer_cast<T>(m_Reference);
	}

	

private:

	// Generate type information
	inline static TypeIdentifier<T> TypeIdentifier{};
};

template <typename T>
std::enable_if<std::is_base_of_v<ComponentBase, T>,
std::ostream&> operator<<(std::ostream& stream, const std::weak_ptr<T>& component)
{
	if (component.expired()) return stream << 0;
	return stream << component.lock()->GetParent()->GetId();
}

template <typename T>
std::enable_if<std::is_base_of_v<ComponentBase, T>,
Deserializer&> operator>>(Deserializer& stream, std::weak_ptr<T>& component)
{
	unsigned int id{};
	(*stream.GetStream()) >> id;

	stream.GetComponentFromObject(std::type_index(typeid(T)), id, &component);
}