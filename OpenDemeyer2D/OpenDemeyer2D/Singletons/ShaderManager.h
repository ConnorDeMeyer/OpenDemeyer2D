#pragma once

#include "UtilityFiles/Singleton.h"
#include "EngineIO/Reflection.h"
#include "Shaders/ShaderBase.h"

#include <unordered_map>

#define SHADERS ShaderManager::GetInstance()

class ShaderManager final : public Singleton<ShaderManager>
{
public:

	template <typename T>
	[[nodiscard]]
	std::enable_if_t<std::is_base_of_v<ShaderBase, T>, T&>
	GetShader()
	{
		auto it = m_Shaders.find(class_id<T>());
		if (it != m_Shaders.end())
		{
			return reinterpret_cast<T&>(*it->second);
		}

		auto pShader = new T();
		m_Shaders.insert({ class_id<T>(), pShader });
		return *pShader;
	}

	template <typename T>
	[[nodiscard]]
	std::enable_if_t<std::is_base_of_v<ShaderBase, T>, const T&>
	GetShader() const
	{
		auto it = m_Shaders.find(class_id<T>());
		if (it != m_Shaders.end())
		{
			return reinterpret_cast<const T&>(*it->second);
		}

		auto pShader = new T();
		m_Shaders.insert({ class_id<T>(), pShader });
		return *pShader;
	}

private:

	std::unordered_map<uint32_t, ShaderBase*> m_Shaders{};

};
