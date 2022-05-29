﻿#pragma once
#include "EngineFiles/ComponentBase.h"

class StageMovement;
class PeterPepper;
class SpriteComponent;

class PPSpriteMovement : public ComponentBase
{
	COMPONENT_BODY(PPSpriteMovement)

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

	float m_OriginalAnimationSpeed{ 1 };

	bool m_isDying{};

};
