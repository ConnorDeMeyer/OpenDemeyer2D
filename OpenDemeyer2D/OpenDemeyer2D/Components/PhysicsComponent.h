#pragma once
#include <b2_body.h>
#include <glm/glm.hpp>

#include "../ComponentBase.h"
#include "../Delegate.h"


class PhysicsComponent : public ComponentBase
{
public:

	virtual ~PhysicsComponent();
	
	void BeginPlay() override;

	/** Get the Box2D body of this component.*/
	b2Body* GetBody() const { return m_pBody; }

	/** Returns the linear velocity of the body.*/
	const glm::vec2& GetLinearVelocity() const;

	/** Returns the angular velocity of the body.*/
	float GetAngularVelocity() const { return m_pBody->GetAngularVelocity(); }

	/** Adds a box fixture to the body.*/
	void SetAsBox(float halfWidth, float halfHeight) const;

	/** Adds a given fixture to the body.*/
	void AddFixture(const b2FixtureDef& fixture) const;

	/**
	 * Fires when the collision component overlaps with another
	 * The physics component parameter is the component it overlapped with
	 */
	Delegate<PhysicsComponent*> OnOverlap;

	/**
	 * Fires when the collision components stops overlapping another component
	 */
	Delegate<PhysicsComponent*> OnEndOverlap;

	/**
	 * Fires when the physics component collides with another.
	 * The first Vec2 is the impulse of the collision aligned with the normal.
	 * The second Vec2 is the impulse of the collision aligned with the tangent.
	 */
	Delegate<PhysicsComponent*, const glm::vec2&, const glm::vec2&> OnHit;

private:

	b2Body* m_pBody{};

};
