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

private:

	void StunEnd();

private:

	int m_Lives{ 5 };

	bool m_isStunned{};


};
