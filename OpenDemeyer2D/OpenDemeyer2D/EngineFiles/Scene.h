#pragma once

#include "Singletons/SceneManager.h"

#include <vector>
#include <memory>
#include <functional>

#include "UtilityFiles/ODArray.h"

#include "EngineIO/Deserializer.h"
#include "PhysicsInterface.h"

class GameObject;
class b2World;

/** Class responsible for managing GameObjects and the GameObject tree.*/
class Scene final
{
	friend Scene& SceneManager::CreateScene(const std::string& name);

	friend class GameObject;

public:

	Scene(const std::string& name);

	/** Deletes every object in the scene tree.*/
	virtual ~Scene();

	Scene(const Scene& other)				= delete;
	Scene(Scene&& other)					= delete;
	Scene& operator=(const Scene& other)	= delete;
	Scene& operator=(Scene&& other)			= delete;

	/** Adds a Scene object to the top of the scene tree.*/
	GameObject* Add(GameObject* pObject);

	/** Flag the object for deletion at the end of the frame.*/
	void DestroyObject(GameObject* pObject);

	/** Immediately destroys the object without waiting until the end of the frame.*/
	void DestroyObjectImmediately(GameObject* pObject);

	/** Updates the scene objects in the scene tree.*/
	void Update(float deltaTime);

	void PreUpdate(bool IsPlaying);

	void AfterUpdate();

	/** Renders the scene objects in the scene tree.*/
	void Render() const;

	/** Returns the name of this scene.*/
	const std::string& GetName() { return m_Name; }

	void ChangeName(const std::string& name);

	const std::vector<GameObject*>& GetSceneTree() const { return m_SceneTree; }

	void Serialize(std::ostream& os) const;

	void Deserialize(Deserializer& is);

	const std::unordered_map<uint32, GameObject*>& GetAllObjects() const { return m_RegisteredObjects; }

	void Copy(Scene* originalScene);

	void BeginPlay() { m_HasBegunPlay = true; }

	bool HasBegunPlay() { return m_HasBegunPlay; }

	void AddToSceneTree(GameObject* go);

	inline PhysicsInterface* GetPhysicsInterface() const { return m_PhysicsInterface.get(); }

private:

	void RegisterObject(GameObject* pObject);

	void UnregisterObject(GameObject* pObject);

private:

	/** Remove the object from the scene tree list*/
	void RemoveObject(GameObject* object);

private:

	std::string m_Name;
	ODArray<GameObject*> m_SceneTree;

	/** References to object that are flagged for deletion.*/
	ODArray<GameObject*> m_DestroyableObjects;
	
	std::vector<GameObject*> m_UninitializedObject;

	std::vector<GameObject*> m_NotBegunObjects;

	std::unordered_map<uint32, GameObject*> m_RegisteredObjects;

	std::unique_ptr<PhysicsInterface> m_PhysicsInterface;

	bool m_HasBegunPlay{};

};

struct ObjectAlreadyInASceneException : public std::exception
{
	const char* what() const override
	{
		return "This Game Object is already present in a game object";
	}
};