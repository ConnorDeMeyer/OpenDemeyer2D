#pragma once

#include "EngineFiles/ComponentBase.h"

class PhysicsComponent;

class PepperSpray : public ComponentBase
{
	COMPONENT_BODY(PepperSpray)

public:

	void Initialize() override;

	void Update(float deltaTime) override;

private:

	void Overlap(PhysicsComponent* other);

private:

	float m_StunTime{2.f};
	float m_TimeAlive{2.f};

};

