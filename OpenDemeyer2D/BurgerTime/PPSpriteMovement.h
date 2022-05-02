#pragma once
#include "Component.h"

class StageMovement;
class PeterPepper;
class SpriteComponent;

class PPSpriteMovement : public Component<PPSpriteMovement>
{
public:

	void BeginPlay() override;

	void Update(float) override;

	void StartDieAnimation();

	void ResetAnimation();

	//const std::string GetComponentName() override { return "PPSpriteMovement"; };

protected:

	PeterPepper* m_pPeterPepper{};

	SpriteComponent* m_pSpriteComponent{};

	StageMovement* m_pStageMovement{};

	bool m_isDying{};

};
