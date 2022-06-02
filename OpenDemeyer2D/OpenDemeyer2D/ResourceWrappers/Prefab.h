#pragma once
#include "EngineFiles/GameObject.h"
#include <memory>
#include <filesystem>

class Prefab final
{
	friend class ResourceManager;

public:

	Prefab() = default;

	Prefab(GameObject* object)
		: m_Object{object}
	{}

	~Prefab() = default;

	Prefab(const Prefab&) = delete;
	Prefab& operator=(const Prefab&) = delete;

	Prefab(Prefab&& other) noexcept
		: m_Object{ std::move(other.m_Object) }
		, m_sourceFile{ std::move(other.m_sourceFile) }
	{}

	Prefab& operator=(Prefab&& other) noexcept
	{
		m_Object = std::move(other.m_Object);
		m_sourceFile = std::move(other.m_sourceFile);
		return *this;
	}

	GameObject* Instantiate() const
	{
		CopyLinker linker{};
		GameObject* go = new GameObject();
		go->Copy(m_Object.get(), &linker);
		return go;
	}

	GameObject* GetGameObject()
	{
		return m_Object.get();
	}

	const std::filesystem::path& GetPath() const { return m_sourceFile; }

	inline bool IsValid() { return m_Object.get(); }
	inline operator bool() { return IsValid(); }

private:

	std::unique_ptr<GameObject> m_Object;
	std::filesystem::path m_sourceFile{};
};