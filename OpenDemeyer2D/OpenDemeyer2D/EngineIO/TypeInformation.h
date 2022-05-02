#pragma once
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <memory>
#include <string>
#include <algorithm>

#include "Singleton.h"
#include "ComponentBase.h"
#include "CustomSerializers.h"
#include "Deserializer.h"
#include "GameObject.h"

class ComponentBase;
class TypeInformation;
class GameObject;

//template <typename T> std::enable_if_t<std::is_base_of_v<ComponentBase, T>, T*> GameObject::AddComponent<T>();


class UserFieldBinder final
{
private:

	struct FieldSerializerBase
	{
		virtual ~FieldSerializerBase() = default;

		virtual void Serialize(std::ostream& os, const void* address) const = 0;
		virtual void DeSerialize(FileDeserializer& is, void* address) const = 0;
	};

	template <typename T>
	struct FieldSerializer final : public FieldSerializerBase
	{
		void Serialize(std::ostream& os, const void* address) const override
		{
			os << *static_cast<const T*>(address);
		}

		void DeSerialize(FileDeserializer& is, void* address) override
		{
			is >> *static_cast<const T*>(address);
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

		void Deserialize(FileDeserializer& is, ComponentBase* pComponent) const
		{
			size_t address = reinterpret_cast<size_t>(pComponent) + offset;
			pSerializer->DeSerialize(is, pComponent);
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
		instance.ClassNameIds.emplace(std::string(typeid(T).name()), std::type_index(typeid(T)));
		instance.ClassIdNames.emplace(std::type_index(typeid(T)), std::string(typeid(T).name()));
		instance.DefaultClass.emplace(std::type_index(typeid(T)), new T());

		instance.ClassGenerator.emplace(std::type_index(typeid(T)), [](GameObject* ob) -> T*
			{
				return ob->AddComponent<T>();
			});

		UserFieldBinder binder{};
		T object{};
		object.DefineUserFields(binder);
		instance.Fields.emplace(std::type_index(typeid(T)), binder);
	}

};

class TypeInformation : public Singleton<TypeInformation>
{
public:
	std::unordered_map<std::type_index, std::function<ComponentBase*(GameObject*)>> ClassGenerator;
	std::unordered_map<std::string, std::type_index> ClassNameIds;
	std::unordered_map<std::type_index, std::string> ClassIdNames;
	std::unordered_map<std::type_index, UserFieldBinder> Fields;
	std::unordered_map<std::type_index, ComponentBase*> DefaultClass;
};