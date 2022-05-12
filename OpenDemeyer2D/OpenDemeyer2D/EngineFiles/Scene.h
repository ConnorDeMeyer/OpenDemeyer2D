#pragma once
#include <b2_world.h>

#include "Singletons/SceneManager.h"
#include <vector>
#include "UtilityFiles/ODArray.h"
#include <memory>
#include <functional>
#include "EngineIO/Deserializer.h"

class GameObject;
class b2World;

/** Class responsible for managing GameObjects and the GameObject tree.*/
class Scene final : public b2ContactListener
{
	friend Scene& SceneManager::CreateScene(const std::string& name);

	friend class GameObject;

public:

	explicit Scene(const std::string& name);

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

	/** Returns the Box2D physics world.*/
	b2World* GetPhysicsWorld() const { return m_pb2World; }

	void ChangeName(const std::string& name);

	const std::vector<GameObject*>& GetSceneTree() const { return m_SceneTree; }

	void Serialize(std::ostream& os) const;

	void Deserialize(Deserializer& is);

	const std::unordered_map<uint32, GameObject*>& GetAllObjects() const { return m_RegisteredObjects; }

	void Copy(Scene* originalScene);

	void BeginPlay() { m_HasBegunPlay = true; }

	bool HasBegunPlay() { return m_HasBegunPlay; }

	void AddToSceneTree(GameObject* go);

private:

	void RegisterObject(GameObject* pObject);

	void UnregisterObject(GameObject* pObject);

	void BeginContact(b2Contact* contact) override;

	void EndContact(b2Contact* contact) override;

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

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

	b2World* m_pb2World{};

	std::unordered_map<uint32, GameObject*> m_RegisteredObjects;

	bool m_HasBegunPlay{};

};

struct ObjectAlreadyInASceneException : public std::exception
{
	const char* what() const override
	{
		return "This Game Object is already present in a game object";
	}
};