#pragma once

#include "Singletons/SceneManager.h"

#include <vector>
#include <memory>
#include <functional>
#include <filesystem>

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
	friend class ResourceManager;

public:

	Scene(const std::string& name);

	/** Deletes every object in the scene tree.*/
	virtual ~Scene();

	Scene(const Scene& other)				= delete;
	Scene(Scene&& other)					= delete;
	Scene& operator=(const Scene& other)	= delete;
	Scene& operator=(Scene&& other)			= delete;

	/** 
	* Creates a Game Object that is managed by the scene
	* @param pParent: Parent of the game object. Will parent to the scene if nullptr
	*/
	GameObject* CreateGameObject(GameObject* pParent = nullptr);

	/** Adds a Scene object to the top of the scene tree.*/
	//GameObject* Add(GameObject* pObject);

	/** Flag the object for deletion at the end of the frame.*/
	void DestroyObject(GameObject* pObject);

	/** Immediately destroys the object without waiting until the end of the frame.*/
	void DestroyObjectImmediately(GameObject* pObject);

	/** Updates the scene objects in the scene tree.*/
	void Update(float deltaTime);

	/** Updates the scene objects before their Update function is called*/
	void PreUpdate(bool IsPlaying);

	/** Updates the scene objects after their Update function was called*/
	void AfterUpdate();

	/** Renders the scene objects in the scene tree.*/
	void Render() const;

	/** Returns the name of this scene.*/
	const std::string& GetName() { return m_Name; }

	/** Changes the name of the scene*/
	void ChangeName(const std::string& name);

	/** Returns the top Game Objects of the scene without their children*/
	const std::vector<GameObject*>& GetSceneTree() const { return m_SceneTree; }

	/** 
	* Outputs the Scene through a stream
	* This includes the Gameobjects and their components
	*/
	void Serialize(std::ostream& os) const;

	/**
	* Will take a stream and convert it into a Scene with GameObject children
	*/
	void Deserialize(Deserializer& is);

	/** Returns a map of all gameobjects in the scene with as key their Object ID*/
	const std::unordered_map<uint32, GameObject*>& GetAllObjects() const { return m_RegisteredObjects; }

	/** Copies the original scene given into this one including the game objects inside*/
	void Copy(Scene* originalScene);

	/** Begins play of the scene and the Game Objects inside*/
	void BeginPlay() { m_HasBegunPlay = true; }

	/** Returns true if the Scene has begun playing*/
	bool HasBegunPlay() { return m_HasBegunPlay; }

	/** Adds an object to the scene if it isn't already in there*/
	void AddToSceneTree(GameObject* go);

	/** Returns the class responsible for managing the Physics API for this scene*/
	inline PhysicsInterface* GetPhysicsInterface() const { return m_PhysicsInterface.get(); }

	inline const std::filesystem::path& GetFilePath() const { return m_FilePath; }

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
	std::vector<GameObject*> m_NewSceneTreeObjects;

	std::unordered_map<uint32, GameObject*> m_RegisteredObjects;

	std::unique_ptr<PhysicsInterface> m_PhysicsInterface;

	std::filesystem::path m_FilePath;

	bool m_HasBegunPlay{};

};

struct ObjectAlreadyInASceneException : public std::exception
{
	const char* what() const override
	{
		return "This Game Object is already present in a game object";
	}
};