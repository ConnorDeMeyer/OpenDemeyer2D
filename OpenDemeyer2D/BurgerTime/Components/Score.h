#pragma once
#include "EngineFiles/ComponentBase.h"
#include "UtilityFiles/EventQueue.h"

class TextPixelComponent;

enum class ScoreEvent
{
	drop_Burger,
	drop_burger_1enemy,
	drop_burger_2enemy,
	drop_burger_3enemy,
	drop_burger_4enemy,
	drop_burger_5enemy,
	drop_burger_6enemy,
	kill_hotDog,
	kill_pickle,
	kill_egg
};

typedef EventQueue<ScoreEvent, glm::vec2> ScoreEventQueue;

class Score final : public ComponentBase
{
	COMPONENT_BODY(Score)

public:

	virtual void DefineUserFields(UserFieldBinder& binder) const;

	void Update(float) override;

	void RenderImGui() override;

private:

	std::weak_ptr<TextPixelComponent> m_pTextComp{};

	Uint32 m_Score{};

	std::deque<std::pair<GameObject*, float>> m_ScoreDisplays;

	std::shared_ptr<Texture2D> m_SheetTexture{};

	static std::unordered_map<int, float> ScoreTextureMap;

};
