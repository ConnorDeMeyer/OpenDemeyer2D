#pragma once
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <memory>
#include <string>
#include <algorithm>
#include <string>
#include <cassert>
#include <type_traits>

#include "UtilityFiles/Singleton.h"
//#include "EngineFiles/ComponentBase.h"
#include "CustomSerializers.h"
#include "Deserializer.h"
#include "EngineFiles/GameObject.h"
#include "Reflection.h"

class ComponentBase;
class TypeInformation;
class GameObject;

template<typename T> struct is_weak_ptr : std::false_type {};
template<typename T> struct is_weak_ptr<std::weak_ptr<T>> : std::true_type {};

/**
* Class responsible for keeping field information of a particular component
*/
class UserFieldBinder final
{
private:

	/**
	* Base class that defines functions that a Serializer should have.
	* It should be able to copy a field from one class to another
	* It should be able to give its value to a stream
	* It should be able to receive a value from a stream and intepret it.
	* Overload the stream operators of a custom type for it to be serializable (have a look at CustomSerializers.h)
	*/
	struct FieldSerializerBase
	{
		virtual ~FieldSerializerBase() = default;

		virtual void Serialize(std::ostream& os, const void* address) const = 0;
		virtual void DeSerialize(Deserializer& is, void* address) const = 0;
		virtual void Copy(const void* originalAddress, void* copyAddress, CopyLinker* copyLinker = nullptr) const = 0;
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
			if constexpr (is_weak_ptr<T>::value) // if it is a weak pointer we have to give the deserializer object instead.
				is >> static_cast<T*>(address);
			else
				(*is.GetStream()) >> *static_cast<T*>(address);
		}

		void Copy(const void* originalAddress, void* copyAddress, CopyLinker* copyLinker = nullptr) const override
		{
			if constexpr (is_weak_ptr<T>::value)
			{
				CopyWeakPtr(*static_cast<const T*>(originalAddress), *static_cast<T*>(copyAddress), copyLinker);
			}
			else if constexpr (std::is_pointer_v<T>)
			{
				*static_cast<T*>(copyAddress) = new std::remove_pointer_t<T>();
				*static_cast<T*>(copyAddress) = **static_cast<const T*>(originalAddress);
			}
			else
			{
				*static_cast<T*>(copyAddress) = *static_cast<const T*>(originalAddress);
			}
		}
	};

	/**
	* Class responsible for keeping field info.
	*/
	struct FieldInfo final
	{
		/** Offset of the field's address relative to the class's address*/
		size_t offset{};
		/** Size of the field*/
		size_t size{};
		/** Serializer containing functions for serializing, deserializing and copying fields*/
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

		void Copy(const ComponentBase* originalComponent, ComponentBase* copyComponent, CopyLinker* copyLinker = nullptr) const
		{
			size_t Originaladdress = reinterpret_cast<size_t>(originalComponent) + offset;
			size_t Copyaddress = reinterpret_cast<size_t>(copyComponent) + offset;
			pSerializer->Copy(reinterpret_cast<void*>(Originaladdress), reinterpret_cast<void*>(Copyaddress), copyLinker);
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

/** Singleton that keeps all the type information*/
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
