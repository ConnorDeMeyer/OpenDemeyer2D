#pragma once
#include <b2_world.h>

#include "SceneManager.h"
#include <vector>
#include "ODArray.h"
#include <memory>

class GameObject;
class b2World;

/** Class responsible for managing GameObjects and the GameObject tree.*/
class Scene final : public b2ContactListener
{
	friend Scene& SceneManager::CreateScene(const std::string& name);

	friend class GameObject;

private:

	explicit Scene(const std::string& name);

public:

	/** Deletes every object in the scene tree.*/
	virtual ~Scene();

	Scene(const Scene& other)				= delete;
	Scene(Scene&& other)					= delete;
	Scene& operator=(const Scene& other)	= delete;
	Scene& operator=(Scene&& other)			= delete;

	/** Adds a Scene object to the top of the scene tree.*/
	void Add(GameObject* pObject);

	/** Flag the object for deletion at the end of the frame.*/
	void DestroyObject(GameObject* pObject);

	/** Immediately destroys the object without waiting until the end of the frame.*/
	void DestroyObjectImmediately(GameObject* pObject);

	/** Updates the scene objects in the scene tree.*/
	void Update(float deltaTime);

	/** Renders the scene objects in the scene tree.*/
	void Render() const;

	/** Renders the ImGui of the gameobjects*/
	void RenderImGui();

	/** Returns the name of this scene.*/
	const std::string& GetName() { return m_Name; }

	/** Returns the Box2D physics world.*/
	b2World* GetPhysicsWorld() const { return m_pb2World; }

private:

	void BeginContact(b2Contact* contact) override;

	void EndContact(b2Contact* contact) override;

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

private:

	/** Remove the object from the scene tree list*/
	void RemoveObject(GameObject* object);

	/** Sets a reference to the scene inside the object and its children*/
	void SetScene(GameObject* object);

private:

	std::string m_Name;
	ODArray<GameObject*> m_SceneTree;

	/** References to object that are flagged for deletion.*/
	ODArray<GameObject*> m_DestroyableObjects;

	b2World* m_pb2World{};
};

struct ObjectAlreadyInASceneException : public std::exception
{
	const char* what() const override
	{
		return "This Game Object is already present in a game object";
	}
};