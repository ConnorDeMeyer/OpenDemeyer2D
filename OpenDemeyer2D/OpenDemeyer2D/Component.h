#pragma once
#include <memory>
#include <string>
#include "Dictionary.h"
#include "Singleton.h"
#include <unordered_map>
#include <functional>
#include <typeindex>

#include "ComponentBase.h"
#include "EngineIO/TypeInformation.h"
#include "EngineIO/Deserializer.h"


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

	void Deserialize(FileDeserializer& is)
	{
		auto& fields = TypeInformation::GetInstance().Fields[typeid(T)].GetFields();
		if (CanContinue(is))
		{
			while (!IsEnd(is))
			{
				std::string fieldName;
				is >> fieldName;
				auto it = fields.find(fieldName);
				if (it != fields.end())
				{
					it->second.Deserialize(is, this);
				}
			}
		}
	}

private:

	// Generate type information
	inline static TypeIdentifier<T> TypeIdentifier{};
};
