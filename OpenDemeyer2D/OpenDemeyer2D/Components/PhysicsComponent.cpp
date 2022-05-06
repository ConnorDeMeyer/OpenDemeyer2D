#include "PhysicsComponent.h"

#include <b2_body.h>
#include <b2_fixture.h>
#include <b2_polygon_shape.h>
#include "Components/Transform.h"

#include "EngineFiles/Scene.h"
#include "Singletons/OpenDemeyer2D.h"


void PhysicsComponent::DefineUserFields(UserFieldBinder&) const
{

}

PhysicsComponent::~PhysicsComponent()
{
	if (GetParent()) {
		auto scene = GetParent()->GetScene();
		if (scene && m_pBody)
		{
			scene->GetPhysicsWorld()->DestroyBody(m_pBody);
		}
	}
}

void PhysicsComponent::BeginPlay()
{
	if (!m_pBody) CreateBody();
}

void PhysicsComponent::Update(float)
{
	auto transform = GetParent()->GetTransform();
	m_pBody->SetTransform(reinterpret_cast<const b2Vec2&>(transform->GetWorldPosition()), transform->GetWorldRotation());
}

const glm::vec2& PhysicsComponent::GetLinearVelocity() const
{
	return reinterpret_cast<const glm::vec2&>(m_pBody->GetLinearVelocity());
}

void PhysicsComponent::AddBox(float halfWidth, float halfHeight, bool isSensor, const glm::vec2& center, float rotation)
{
	if (!m_pBody) CreateBody();

	b2PolygonShape boxShape{};
	boxShape.SetAsBox(halfWidth, halfHeight, { center.x, center.y }, rotation);

	b2FixtureDef def{};
	def.userData.pointer = uintptr_t(this);
	def.isSensor = isSensor;
	def.shape = &boxShape;

	m_pBody->CreateFixture(&def);
}

void PhysicsComponent::AddFixture(b2FixtureDef& fixture)
{
	fixture.userData.pointer = uintptr_t(this);
	m_pBody->CreateFixture(&fixture);
}

void PhysicsComponent::CreateBody()
{
	auto& position = GetParent()->GetTransform()->GetWorldPosition();

	b2BodyDef bodyDef{};
	bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
	bodyDef.position.Set(position.x, position.y);
	bodyDef.type = b2_dynamicBody;
	bodyDef.allowSleep = false;

	m_pBody = GetParent()->GetScene()->GetPhysicsWorld()->CreateBody(&bodyDef);
}
