#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include "EngineIO/Reflection.h"

/**
 * This is a map that can store any type of value.
 **/

class Dictionary final
{
public:
	Dictionary() = default;
	~Dictionary()
	{
		for (auto& t : m_DictionaryValues)
			delete t.second;
	}

	Dictionary(const Dictionary& other) = delete;
	Dictionary& operator=(const Dictionary& other) = delete;
	Dictionary(Dictionary&& other) = delete;
	Dictionary& operator=(Dictionary&& other) = delete;

	template<typename T>
	struct EntryStruct
	{
		EntryStruct(const std::string& _key, const T& _value)
			: key{_key}
			, value{_value}
		{}

		std::string key;
		T value;
	};

	template<typename T>
	void InsertEntry(const EntryStruct<T>& entry)
	{
		Insert(entry.key, entry.value);
	}

	template<typename Type0, typename... OtherTypes>
	void InsertEntry(const EntryStruct<Type0>& entry0, const EntryStruct<OtherTypes>&... otherEntries)
	{
		InsertEntry<Type0>(entry0);
		InsertEntry<OtherTypes...>(otherEntries...);
	}

	template<typename T>
	Dictionary(const EntryStruct<T>& entry)
	{
		InsertEntry(entry);
	}

	template<typename Type0, typename... OtherTypes>
	Dictionary(const EntryStruct<Type0>& entry0, const EntryStruct<OtherTypes>&... otherEntries)
	{
		InsertEntry(entry0, otherEntries...);
	}

public:

	class IDictionaryEntry
	{
	public:
		IDictionaryEntry() = default;

		virtual ~IDictionaryEntry() = default;

		/** used to input a value using a stream*/
		virtual void IStreamValue(std::istream& is) = 0;

		/** used to output a value using a stream*/
		virtual void OStreamValue(std::ostream& os) = 0;

		virtual uint32_t GetTypeId() const = 0;

		/*virtual void BinaryWrite(std::istream& is) = 0;

		virtual void BinaryRead(std::ostream& os) = 0;*/
		
	};

public:

	template<typename T>
	class DictionaryEntry final : public IDictionaryEntry
	{
	public:
		explicit DictionaryEntry(const T& value) : m_Value{value} {}
		T& GetValue() { return m_Value; }

		// Safe setter
		void SetValue(const T& value) { m_Value = value; }

		void IStreamValue(std::istream& is) override
		{
			//if constexpr (std::is_trivially_copyable_v<T>)
			//{
			//	char* address = reinterpret_cast<char*>(&m_Value);
			//	is.readsome(address, sizeof(T));
			//}
			//else
			{
				is >> m_Value;
			}
		}

		void OStreamValue(std::ostream& os) override
		{
			//if constexpr (std::is_trivially_copyable_v<T>)
			//{
			//	char* address = reinterpret_cast<char*>(&m_Value);
			//	os.write(address, sizeof(T));
			//}
			//else
			{
				os << m_Value;
			}
		}

		uint32_t GetTypeId() const override
		{
			constexpr uint32_t id = class_id<T>();
			return id;
		}

	private:
		T m_Value;
	};

public:

	template<typename T>
	bool Insert(const std::string& key, const T& value)
	{
		// add the value if the key doesn't exist yet
		auto it = m_DictionaryValues.find(key);
		if (it == m_DictionaryValues.end())
		{
			m_DictionaryValues.insert(std::make_pair(key, new DictionaryEntry<T>(value)));
			return true;
		}
		return false;
	}

	template<typename T>
	bool ChangeData(const std::string& key, const T& data)
	{
		auto it = m_DictionaryValues.find(key);
		if (it != m_DictionaryValues.end())
		{
			constexpr uint32_t id{ class_id<T>() };
			auto value = *it;
			if (id == value.second->GetTypeId())
			{
				reinterpret_cast<DictionaryEntry<T>*>(value.second)->SetValue(data);
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool GetData(const std::string& key, T& data) const
	{
		constexpr uint32_t id{ class_id<T>() };
		auto value = m_DictionaryValues.at(key);
		if (id == value->GetTypeId())
		{
			data = reinterpret_cast<DictionaryEntry<T>*>(value)->GetValue();
			return true;
		}
		return false;
	}

	/** Will use the << operator twice: first to get the key and then to get the value if it found the key*/
	bool StreamChange(std::istream& input)
	{
		std::string key;
		input >> key;
		
		auto it = m_DictionaryValues.find(key);
		if (it != m_DictionaryValues.end())
		{
			it->second->IStreamValue(input);
			return true;
		}
		return false;
	}

	bool StreamChange(const std::string& key, std::istream& input)
	{
		auto it = m_DictionaryValues.find(key);
		if (it != m_DictionaryValues.end())
		{
			it->second->IStreamValue(input);
			return true;
		}
		return false;
	}

	auto begin() const { return m_DictionaryValues.begin(); }
	auto end() const { return m_DictionaryValues.end(); }
	auto size() const { return m_DictionaryValues.size(); }



private:
	
	std::unordered_map<std::string, IDictionaryEntry*> m_DictionaryValues;

};