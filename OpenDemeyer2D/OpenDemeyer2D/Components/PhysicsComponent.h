#pragma once
#include <b2_body.h>
#include <glm/glm.hpp>

#include "EngineFiles/Component.h"
#include "UtilityFiles/Delegate.h"


class PhysicsComponent : public Component<PhysicsComponent>
{
public:

	virtual void DefineUserFields(UserFieldBinder& binder) const;

	virtual ~PhysicsComponent();
	
	void BeginPlay() override;

	void Update(float) override;

	/** Get the Box2D body of this component.*/
	b2Body* GetBody() const { return m_pBody; }

	/** Returns the linear velocity of the body.*/
	const glm::vec2& GetLinearVelocity() const;

	/** Returns the angular velocity of the body.*/
	float GetAngularVelocity() const { return m_pBody->GetAngularVelocity(); }

	/** Adds a box fixture to the body.*/
	void AddBox(float halfWidth, float halfHeight, bool isSenor = false, const glm::vec2& center = {0,0}, float rotation = 0);

	/** Adds a given fixture to the body.*/
	void AddFixture(b2FixtureDef& fixture);

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

	//const std::string GetComponentName() override { return "PhysicsComponent"; }

private:

	void CreateBody();

	b2Body* m_pBody{};

};
