#pragma once
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <memory>
#include <string>
#include <algorithm>
#include <string>
#include <cassert>

#include "UtilityFiles/Singleton.h"
#include "EngineFiles/ComponentBase.h"
#include "CustomSerializers.h"
#include "Deserializer.h"
#include "EngineFiles/GameObject.h"
#include "Reflection.h"

class ComponentBase;
class TypeInformation;
class GameObject;


class UserFieldBinder final
{
private:

	struct FieldSerializerBase
	{
		virtual ~FieldSerializerBase() = default;

		virtual void Serialize(std::ostream& os, const void* address) const = 0;
		virtual void DeSerialize(Deserializer& is, void* address) const = 0;
	};

	template <typename T>
	struct FieldSerializer final : public FieldSerializerBase
	{
		void Serialize(std::ostream& os, const void* address) const override
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				if (static_cast<const std::string*>(address)->empty())
				{
					os << "text";
					return;
				}
			}
			os << *static_cast<const T*>(address);
		}

		void DeSerialize(Deserializer& is, void* address) const override
		{
			(*is.GetStream()) >> *static_cast<T*>(address);
		}
	};

	struct FieldInfo final
	{
		size_t offset{};
		size_t size{};
		std::shared_ptr<FieldSerializerBase> pSerializer{};

		void Serialize(std::ostream& os, const ComponentBase* pComponent) const
		{
			size_t address = reinterpret_cast<size_t>(pComponent) + offset;
			pSerializer->Serialize(os, reinterpret_cast<const void*>(address));
		}

		void Deserialize(Deserializer& is, ComponentBase* pComponent) const
		{
			size_t address = reinterpret_cast<size_t>(pComponent) + offset;
			pSerializer->DeSerialize(is, reinterpret_cast<void*>(address));
		}
	};

public:
	UserFieldBinder() = default;

	template <typename Type>
	void Add(const std::string& identifier, size_t fieldOffset)
	{
		assert(std::find_if(identifier.begin(), identifier.end(), [](char c) {return isspace(c); }) == identifier.end());
		FieldInfo info = FieldInfo{fieldOffset, sizeof(Type), std::shared_ptr<FieldSerializerBase>(new FieldSerializer<Type>()) };
		m_UserFields.insert({ identifier, std::move(info) });
	}

	const std::unordered_map<std::string, FieldInfo>& GetFields() const { return m_UserFields; }

private:

	std::unordered_map<std::string, FieldInfo> m_UserFields;

};

template <typename T>
class TypeIdentifier final
{
public:

	TypeIdentifier()
	{
		auto& instance = TypeInformation::GetInstance();
		constexpr std::string_view name = type_name<T>();

		std::function<ComponentBase* (GameObject*)> generator;
		if constexpr (std::is_same_v<Transform, T>)
		{
			generator = [](GameObject* ob) -> T*
			{
				return ob->GetTransform();
			};
		}
		else
		{
			generator = [](GameObject* ob) -> T*
			{
				return ob->AddComponent<T>();
			};
		}

		UserFieldBinder binder{};
		T object{};
		object.DefineUserFields(binder);

		instance.AddTypeInfo(hash(name), {name,generator,binder});
	}

};

class TypeInformation : public Singleton<TypeInformation>
{
public:

	struct TypeInfo
	{
		std::string_view name;
		std::function<ComponentBase* (GameObject*)> componentGenerator;
		UserFieldBinder field;
	};

public:

	void AddTypeInfo(uint32_t id,TypeInfo&& info)
	{
		auto it = m_TypeInfo.find(id);
		if (it != m_TypeInfo.end())
			assert(false && "Two types have same hash");
		m_ClassNameIds.emplace(info.name, id);
		m_TypeInfo.emplace(id, info);
	}

	TypeInfo* GetTypeInfo(const std::string& typeName)
	{
		auto it = m_ClassNameIds.find(typeName);
		if (it != m_ClassNameIds.end())
		{
			auto typeIt = m_TypeInfo.find(it->second);
			if (typeIt != m_TypeInfo.end())
			{
				return &typeIt->second;
			}
		}
		return nullptr;
	}

	TypeInfo* GetTypeInfo(uint32_t id)
	{
		const auto it = m_TypeInfo.find(id);
		if (it != m_TypeInfo.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	const std::unordered_map<uint32_t, TypeInfo>& GetTypeInfos() const { return m_TypeInfo; }

private:

	std::unordered_map<std::string, uint32_t > m_ClassNameIds;
	std::unordered_map<uint32_t, TypeInfo> m_TypeInfo;

};
