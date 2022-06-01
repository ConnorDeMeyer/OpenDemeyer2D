#pragma once
#include "EngineFiles/ComponentBase.h"
#include "glm/glm.hpp"
#include "UtilityFiles/Delegate.h"


class PeterPepper final : public ComponentBase
{
	COMPONENT_BODY(PeterPepper)

public:

	void BeginPlay() override;

	int GetLives() const { return m_Lives; }

	Delegate<> OnLifeLost;

	void LoseLife();

	Delegate<> OnPepperSpray;

	int GetPepperAmount() const { return m_Pepper; }

	void RenderImGui() override;

private:

	void StunEnd();

private:

	int m_Lives{ 3 };

	int m_Pepper{ 5 };

	bool m_isStunned{};

	std::shared_ptr<GameObject> m_PepperPrefab;
};
