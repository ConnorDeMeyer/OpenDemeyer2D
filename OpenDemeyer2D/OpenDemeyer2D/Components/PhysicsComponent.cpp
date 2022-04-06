#include "PhysicsComponent.h"

#include <b2_body.h>
#include <b2_polygon_shape.h>

#include "../Scene.h"


PhysicsComponent::~PhysicsComponent()
{
	auto scene = GetParent()->GetScene();
	if (scene && m_pBody)
	{
		scene->GetPhysicsWorld()->DestroyBody(m_pBody);
	}
}

void PhysicsComponent::BeginPlay()
{
	if (!m_pBody) CreateBody();
}

const glm::vec2& PhysicsComponent::GetLinearVelocity() const
{
	return reinterpret_cast<const glm::vec2&>(m_pBody->GetLinearVelocity());
}

void PhysicsComponent::SetAsBox(float halfWidth, float halfHeight)
{
	if (!m_pBody) CreateBody();

	b2PolygonShape boxShape{};
	boxShape.SetAsBox(halfWidth, halfHeight);
	
	m_pBody->CreateFixture(&boxShape, 0.f);
}

void PhysicsComponent::AddFixture(const b2FixtureDef& fixture)
{
	m_pBody->CreateFixture(&fixture);
}

void PhysicsComponent::CreateBody()
{
	auto& position = GetParent()->GetTransform()->GetWorldPosition();

	b2BodyDef bodyDef{};
	bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
	bodyDef.position.Set(position.x, position.y);

	m_pBody = GetParent()->GetScene()->GetPhysicsWorld()->CreateBody(&bodyDef);
}
