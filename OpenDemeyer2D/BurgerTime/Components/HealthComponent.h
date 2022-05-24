#pragma once

#include "EngineFiles/ComponentBase.h"
#include "UtilityFiles/Delegate.h"

class HealthComponent : public ComponentBase
{
	COMPONENT_BODY(HealthComponent)

public:

	void Damage(float damage);
	void Heal(float amount);

	void SetHealth(float health);
	void SetMaxHealth(float maxHealth) { m_MaxHealth = maxHealth; }

	inline float GetHealth() const { return m_Health; }
	inline float GetMaxHealth() const { return m_MaxHealth; }

	Delegate<> OnDeath;

	Delegate<float> OnDamage;

	Delegate<float> OnHeal;

private:

	float m_Health;
	float m_MaxHealth;
};
