#pragma once
#include <ComponentBase.h>

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
public:

	void BeginPlay() override;

	void SetType(BurgerPieceType type);


private:

	void SegmentOverlap(GameObject* pSegment, PhysicsComponent* other);

private:

	GameObject* m_pSegments[4]{};

	bool m_HitSegments[4]{};

	BurgerPieceType m_Type{};

};



