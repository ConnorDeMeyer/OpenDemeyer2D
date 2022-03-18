#include "HealthComponent.h"

void HealthComponent::Damage(float damage)
{
	if (damage <= 0) return;

	m_Health -= damage;
	OnDamage.BroadCast(damage);

	if (m_Health <= 0)
	{
		m_Health = 0;
		OnDeath.BroadCast();
	}
}

void HealthComponent::Heal(float amount)
{
	if (amount <= 0) return;

	m_Health = std::min(m_Health + amount, m_MaxHealth);
	OnHeal.BroadCast(amount);
}
