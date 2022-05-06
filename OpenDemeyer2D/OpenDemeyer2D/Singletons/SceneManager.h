#pragma once

#include "UtilityFiles/Singleton.h"
#include <string>
#include <vector>

#define SCENES SceneManager::GetInstance()

class Scene;
class SceneManager final : public Singleton<SceneManager>
{
	friend class Singleton<SceneManager>;

public:

	/** Updates all the scenes*/
	void Update(float deltaTime);

	void PreUpdate(bool isPlaying);

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

	/** Renders the ImGui of the scene*/
	void RenderImGui();

	/** Returns the scene that is currently being updated and rendered*/
	Scene* GetActiveScene() const { return m_pActiveScene; }

	void PhysicsStep(float timeStep, int velocityIterations, int positionIterations);

	void RemoveScene(Scene* pScene);

	void RemoveAllScenes();

	void RemoveActiveScene();

	void Serialize(std::ostream& os);

private:

	SceneManager() = default;

	virtual ~SceneManager();

private:

	std::vector<Scene*> m_Scenes;

	Scene* m_pActiveScene{};

};

