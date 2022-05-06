#pragma once
#include "EngineFiles/Component.h"
#include "glm/glm.hpp"
#include "UtilityFiles/Delegate.h"


class PeterPepper final : public Component<PeterPepper>
{
public:

	void BeginPlay() override;

	int GetLives() const { return m_Lives; }

	Delegate<> OnLifeLost;

	Delegate<int> OnScoreGain;

	int GetScore() const { return m_Score; }

	void LoseLife();

	void GainScore(int amount);

	//const std::string GetComponentName() override { return "PeterPepper"; }

protected:

	int m_Lives{ 5 };

	int m_Score{};


};
