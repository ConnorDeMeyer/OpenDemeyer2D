#pragma once
#include "ComponentBase.h"
#include "glm/glm.hpp"
#include "Delegate.h"

class PeterPepper final : public ComponentBase
{
public:

	void BeginPlay() override;

	void MoveRight(float Value);

	const glm::vec2& GetDirection() const { return m_Direction; }

	int GetLives() const { return m_Lives; }

	Delegate<> OnLifeLost;

	Delegate<int> OnScoreGain;

	int GetScore() const { return m_Score; }

	void LoseLife();

	void GainScore(int amount);

protected:

	glm::vec2 m_Direction{};

	float m_MovementSpeed{ 1.f };

	int m_Lives{ 5 };

	int m_Score{};

};
