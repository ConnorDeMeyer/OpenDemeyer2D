#pragma once
#include "EngineFiles/ComponentBase.h"

class PhysicsComponent;
class BurgerPieceSegment;
class TextureComponent;
class RenderComponent;
class Stage;

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

	void Initialize() override;

	void BeginPlay() override;

	void SetType(BurgerPieceType type);

	void Update(float) override;

	void RenderImGui() override;

	void DefineUserFields(UserFieldBinder& binder) const override;

	inline bool IsFalling() const { return m_IsFalling; }

	inline int GetPositionInStack() const { return m_PositionInStack; }

private:

	void FallDown();
	void FallDown(const glm::vec2& location);

	void SegmentOverlap(GameObject* pSegment, PhysicsComponent* other);

private:

	GameObject* m_pSegments[4]{};

	std::weak_ptr<Stage> m_Stage;

	bool m_HitSegments[4]{};

	float m_RestHeight{};
	float m_FallDelay{};
	
	int m_PositionInStack{};
	
	BurgerPieceType m_Type{};

	bool m_IsFalling{};


};



