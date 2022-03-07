#pragma once

#include "Singleton.h"
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

	/** Renders all the scenes*/
	void Render() const;

	/** Creates a scene and add it to the scenes list*/
	Scene& CreateScene(const std::string& name);

	/** Returns the scene corresponding to the given name (or nullptr if no scene found).*/
	Scene* GetScene(const std::string& name) const;

	void SetActiveScene(const std::string& name);

	void SetActiveScene(Scene* pScene);

	/** Renders the ImGui of the scene*/
	void RenderImGui();

private:

	SceneManager() = default;

	virtual ~SceneManager();

	std::vector<Scene*> m_Scenes;

	Scene* m_pActiveScene{};

};

