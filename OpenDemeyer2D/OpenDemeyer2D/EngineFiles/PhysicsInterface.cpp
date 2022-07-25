#include "pch.h"
#include "PhysicsInterface.h"

#include "Components/PhysicsComponent.h"

PhysicsInterface::PhysicsInterface()
{
	m_pb2World = new b2World(b2Vec2{ 0, 0 });
	m_pb2World->SetContactListener(this);
}

PhysicsInterface::~PhysicsInterface()
{
	delete m_pb2World;
}

void PhysicsInterface::DestroyBody(PhysicsComponent* pComp)
{
	m_OverlappingComponents.erase(m_OverlappingComponents.find(pComp));
	
	DestroyBody(pComp->GetBody());
}

void PhysicsInterface::DestroyBody(b2Body* pBody)
{
	m_pb2World->DestroyBody(pBody);
}

b2Body* PhysicsInterface::CreateBody(const b2BodyDef& def)
{
	return m_pb2World->CreateBody(&def);
}

const ODArray<PhysicsComponent*>& PhysicsInterface::GetOverlappingComponents(PhysicsComponent* pComp)
{
	return m_OverlappingComponents[pComp];
}

void PhysicsInterface::BeginContact(b2Contact* contact)
{
	PhysicsComponent* compA = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureA()->GetUserData().pointer);
	PhysicsComponent* compB = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureB()->GetUserData().pointer);

	if (compA && compB) {
 		compA->OnOverlap.BroadCast(compB);
		compB->OnOverlap.BroadCast(compA);

		if (contact->GetFixtureA()->IsSensor())
			m_OverlappingComponents[compA].emplace_back(compB);

		if (contact->GetFixtureB()->IsSensor())
			m_OverlappingComponents[compB].emplace_back(compA);
	}
}

void PhysicsInterface::EndContact(b2Contact* contact)
{
	PhysicsComponent* compA = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureA()->GetUserData().pointer);
	PhysicsComponent* compB = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureB()->GetUserData().pointer);

	if (compA && compB) {
		compA->OnEndOverlap.BroadCast(compB);
		compB->OnEndOverlap.BroadCast(compA);

		if (contact->GetFixtureA()->IsSensor())
			m_OverlappingComponents[compA].SwapRemove(compB);

		if (contact->GetFixtureB()->IsSensor())
			m_OverlappingComponents[compB].SwapRemove(compA);
	}
}

void PhysicsInterface::PreSolve(b2Contact*, const b2Manifold*)
{

}

void PhysicsInterface::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	PhysicsComponent* compA = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureA()->GetUserData().pointer);
	PhysicsComponent* compB = reinterpret_cast<PhysicsComponent*>(contact->GetFixtureB()->GetUserData().pointer);

	if (compA && compB) {
		compA->OnHit.BroadCast(compB,
			reinterpret_cast<const glm::vec2&>(impulse->normalImpulses),
			reinterpret_cast<const glm::vec2&>(impulse->tangentImpulses));
	}
}