#include "PepperSpray.h"

#include "Components/PhysicsComponent.h"
#include "Enemy.h"
#include "EngineFiles/ComponentBase.h"

using namespace std::placeholders;


void PepperSpray::Initialize()
{
	if (auto physics = GetComponent<PhysicsComponent>())
	{
		physics->OnOverlap.BindFunction(this, std::bind(&PepperSpray::Overlap, this, _1));
	}
}

void PepperSpray::Update(float deltaTime)
{
	m_TimeAlive -= deltaTime;
	if (m_TimeAlive <= 0)
	{
		GetGameObject()->Destroy();
	}
}

void PepperSpray::Overlap(PhysicsComponent* other)
{
	if (auto enemy = other->GetGameObject()->GetComponent<Enemy>())
	{
		enemy->Stun(m_StunTime);
	}
}
