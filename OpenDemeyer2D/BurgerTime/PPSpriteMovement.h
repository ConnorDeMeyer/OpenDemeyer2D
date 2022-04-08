#pragma once
#include "ComponentBase.h"

class PeterPepper;
class SpriteComponent;

class PPSpriteMovement : public ComponentBase
{
public:

	void BeginPlay() override;

	void Update(float) override;

	void StartDieAnimation();

	void ResetAnimation();

	const std::string GetComponentName() override { return "PPSpriteMovement"; };

protected:

	PeterPepper* m_pPeterPepper{};

	SpriteComponent* m_pSpriteComponent{};

	bool m_isDying{};

};
