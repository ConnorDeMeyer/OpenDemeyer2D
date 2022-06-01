#pragma once
#include <b2_world.h>
#include <unordered_map>
#include "UtilityFiles/ODArray.h"

class PhysicsComponent;

class PhysicsInterface final : public b2ContactListener
{
public:

	PhysicsInterface();
	virtual ~PhysicsInterface();

	/** Returns the Box2D physics world.*/
	b2World* GetPhysicsWorld() const { return m_pb2World; }

	void DestroyBody(PhysicsComponent* pComp);
	void DestroyBody(b2Body* pBody);

	b2Body* CreateBody(const b2BodyDef& def);

	const ODArray<PhysicsComponent*>& GetOverlappingComponents(PhysicsComponent* pComp);

private:

	void BeginContact(b2Contact* contact) override;

	void EndContact(b2Contact* contact) override;

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

private:

	b2World* m_pb2World{};

	/** List of sensors and the components that they overlap*/
	std::unordered_map<PhysicsComponent*, ODArray<PhysicsComponent*>> m_OverlappingComponents;

};

