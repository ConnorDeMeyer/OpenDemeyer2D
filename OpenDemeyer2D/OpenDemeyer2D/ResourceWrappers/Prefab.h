#pragma once
#include "EngineFiles/GameObject.h"
#include <memory>
#include <filesystem>

class Prefab final
{
	friend class ResourceManager;

public:

	Prefab(GameObject* object)
		: m_Object{object}
	{}

	~Prefab() = default;

	Prefab(const Prefab&) = delete;
	Prefab(Prefab&&) = delete;
	Prefab& operator=(const Prefab&) = delete;
	Prefab& operator=(Prefab&&) = delete;

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

private:

	std::unique_ptr<GameObject> m_Object;
	std::filesystem::path m_sourceFile{};
};