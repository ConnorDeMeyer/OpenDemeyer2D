#pragma once

#include "UtilityFiles/Singleton.h"
#include <string>
#include <vector>
#include <memory>

#define SCENES SceneManager::GetInstance()

class Scene;
class GameObject;

class SceneManager final : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;

public:

	/** Updates all the scenes*/
	void Update(float deltaTime);

	void PreUpdate();

	void AfterUpdate();

	/** Renders all the scenes*/
	void Render() const;

	/** 
	* Creates a scene and add it to the scenes list.
	* No white space allowed for the name.
	*/
	Scene& CreateScene(const std::string& name);

	/** Returns the scene corresponding to the given name (or nullptr if no scene found).*/
	Scene* GetScene(const std::string& name) const;

	const std::vector<Scene*>& GetScenes() const { return m_Scenes; }

	void SetActiveScene(const std::string& name);

	void SetActiveScene(Scene* pScene);

	/** Returns the scene that is currently being updated and rendered*/
	Scene* GetActiveScene() const;

	void PhysicsStep(float timeStep, int velocityIterations, int positionIterations);

	void RemoveScene(Scene* pScene);

	void RemoveAllScenes();

	void RemoveActiveScene();

	void Serialize(std::ostream& os);

	/** 
	* Start playing the scene.
	* If pScene is nullptr it will start playing the active scene.
	*/
	void PlayScene(Scene* pScene = nullptr);

	void StopPlayingScene();

	const std::unique_ptr<Scene>& GetGameScene() const { return m_GameScene; }

private:

	SceneManager() = default;

	virtual ~SceneManager();

private:

	std::vector<Scene*> m_Scenes;

	std::unique_ptr<Scene> m_GameScene;

	std::vector<GameObject*> m_Prefabs;

	Scene* m_pActiveScene{};

};

