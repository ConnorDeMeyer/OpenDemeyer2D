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

	const std::string GetComponentName() const override { return typeid(T).name(); }

	std::type_index GetComponentId() const override { return std::type_index(typeid(T)); }

	virtual void DefineUserFields(UserFieldBinder&) const {};

	void Serialize(std::ostream& os) const override
	{
		auto& fields = TypeInformation::GetInstance().Fields[typeid(T)];
		for (auto& field : fields.GetFields())
		{
			os << field.first << ' '; 
			field.second.Serialize(os, this);
			os << '\n';
		}
	}

	void Deserialize(Deserializer& is) override
	{
		auto& fields = TypeInformation::GetInstance().Fields[typeid(T)].GetFields();
		if (CanContinue(*is.GetStream()))
		{
			while (!IsEnd(*is.GetStream()))
			{
				std::string fieldName;
				*is.GetStream() >> fieldName;
				auto it = fields.find(fieldName);
				if (it != fields.end())
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