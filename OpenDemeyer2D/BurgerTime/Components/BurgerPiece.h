#pragma once
#include "EngineFiles/ComponentBase.h"

class PhysicsComponent;
class BurgerPieceSegment;
class TextureComponent;
class RenderComponent;

enum class BurgerPieceType : uint8_t
{
	topBun		= 0,
	bottomBun	= 1,
	cheese		= 2,
	meat		= 3,
	tomato		= 4,
	lettuce		= 5
};

class BurgerPiece final : public ComponentBase
{
	COMPONENT_BODY(BurgerPiece)

public:

	void BeginPlay() override;

	void SetType(BurgerPieceType type);

	void Update(float) override;

	//const std::string GetComponentName() override { return "BurgerPiece"; }

private:

	void FallDown();

	void SegmentOverlap(GameObject* pSegment, PhysicsComponent* other);

private:

	GameObject* m_pSegments[4]{};

	bool m_HitSegments[4]{};

	float m_RestHeight{};

	float m_FallDelay{};

	BurgerPieceType m_Type{};

	bool m_IsFalling{};


};



