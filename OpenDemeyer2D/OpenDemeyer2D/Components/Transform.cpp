#include "Transform.h"

#include "../GameObject.h"

void Transform::SetPosition(const glm::vec2& pos)
{
	glm::vec2 difference = pos - m_Position;
	Move(difference);
}

void Transform::SetScale(const glm::vec2& scale)
{
	glm::vec2 difference = scale / m_Scale;
	Scale(difference);
}

void Transform::Move(const glm::vec2& distance)
{
	m_Position += distance;
	for (GameObject* object : GetParent()->GetChildren())
		object->GetTransform()->Move(distance);
}

void Transform::Scale(const glm::vec2& scalar)
{
	m_Scale *= scalar;
	for (GameObject* object : GetParent()->GetChildren())
		object->GetTransform()->Scale(scalar);
}

void Transform::Scale(float scale)
{
	Scale({ scale, scale });
}

void Transform::AddScale(const glm::vec2& scale)
{
	m_Scale += scale;
	for (GameObject* object : GetParent()->GetChildren())
		object->GetTransform()->AddScale(scale);
}

void Transform::AddScale(float scale)
{
	AddScale({ scale,scale });
}
