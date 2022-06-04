#pragma once
#include "EngineFiles/ComponentBase.h"
#include "UtilityFiles/Delegate.h"

class Prefab;

class PeterPepper final : public ComponentBase
{
	COMPONENT_BODY(PeterPepper)

public:

	void DefineUserFields(UserFieldBinder&) const override;

	void BeginPlay() override;

	int GetLives() const { return m_Lives; }

	Delegate<> OnLifeLost;

	void LoseLife();

	Delegate<> OnPepperSpray;

	int GetPepperAmount() const { return m_Pepper; }

	void RenderImGui() override;

private:

	void StunEnd();

	void ThrowPepper();

private:

	int m_Lives{ 3 };

	int m_Pepper{ 5 };

	bool m_isStunned{};

	std::shared_ptr<Prefab> m_PepperPrefab;
};
