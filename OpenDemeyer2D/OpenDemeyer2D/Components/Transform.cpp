#include "../OD2pch.h"
#include "Transform.h"

#include "../GameObject.h"
#include "imgui.h"

void Transform::BeginPlay()
{
	m_Transformation = m_localTransformation;
}

void Transform::RenderImGui()
{
	// Display Position
	glm::vec2 position = m_Position;
	ImGui::InputFloat2("Position", reinterpret_cast<float*>(&position));

	// Change Position
	if (position != m_Position)
		SetPosition(position);

	// Display scale
	glm::vec2 scale = m_Scale;
	ImGui::InputFloat2("Scale", reinterpret_cast<float*>(&scale));

	// Change scale
	if (scale != m_Scale)
		SetScale(scale);

	// Display rotation
	float rotation = m_Rotation;
	ImGui::InputFloat("Rotation", &rotation);

	// Change rotation
	if (m_Rotation != rotation)
		SetRotation(rotation);
}

const glm::vec2& Transform::GetPosition() const
{
	return m_Position;
}

const glm::vec2& Transform::GetScale() const
{
	return m_Scale;
}

float Transform::GetRotation() const
{
	return m_Rotation;
}

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

void Transform::SetRotation(float rotation)
{
	float difference = rotation - m_Rotation;

	Rotate(difference);
}

void Transform::Move(const glm::vec2& distance)
{
	m_Position += distance;
	m_localTransformation = TransformationMatrix(m_Position, m_Scale, m_Rotation);

	//glm::mat3x3 mat = TranslationMatrix(distance);
	//ApplyMatrix(mat);
	/*m_Position += distance;
	for (GameObject* object : GetParent()->GetChildren())
		object->GetTransform()->Move(distance);*/
}

void Transform::Scale(const glm::vec2& scalar)
{
	m_Scale *= scalar;
	m_localTransformation = TransformationMatrix(m_Position, m_Scale, m_Rotation);

	//glm::mat3x3 mat = ScaleMatrix(scalar);
	//ApplyMatrix(mat);
	/*m_Scale *= scalar;
	for (GameObject* object : GetParent()->GetChildren())
		object->GetTransform()->Scale(scalar);*/
}

void Transform::Rotate(float rotation)
{
	m_Rotation += rotation;
	m_localTransformation = TransformationMatrix(m_Position, m_Scale, m_Rotation);

	//glm::mat3x3 mat = RotationMatrix(rotation);
	//ApplyMatrix(mat);
}

void Transform::Scale(float scale) { Scale({ scale, scale }); }

void Transform::AddScale(const glm::vec2& scale){ Scale({ (m_Scale + scale) / m_Scale }); }

void Transform::AddScale(float scale){ AddScale({ scale,scale }); }

void Transform::ApplyMatrix(const glm::mat3x3& matrix)
{
	m_Transformation = m_Transformation * m_localTransformation;

	for (GameObject* object : GetParent()->GetChildren())
	{
		object->GetTransform()->ApplyMatrix(matrix);
	}
}

glm::vec2 GetPosFromMat(const glm::mat3x3& matrix)
{
	return { matrix[2][0], matrix[2][1] };
}

glm::vec2 GetScaleFromMat(const glm::mat3x3& matrix)
{
	const float SquareSum0 = (matrix[0][0] * matrix[0][0]) + (matrix[0][1] * matrix[0][1]);
	const float SquareSum1 = (matrix[1][0] * matrix[1][0]) + (matrix[1][1] * matrix[1][1]);

	return
	{ glm::sign(matrix[0][0]) * glm::sqrt(SquareSum0),
		glm::sign(matrix[1][1]) * glm::sqrt(SquareSum1) };
}

float GetRotationFromMat(const glm::mat3x3& matrix)
{
	return atan2(matrix[1][0], matrix[1][1]);
}

glm::mat3x3 TranslationMatrix(const glm::vec2& translation)
{
	return glm::mat3x3{
		0,0,translation.x,
		0,0,translation.y,
		0,0,1
	};
}

glm::mat3x3 ScaleMatrix(const glm::vec2& scale)
{
	return glm::mat3x3{
		scale.x,0,0,
		0,scale.y,0,
		0,0,1
	};
}

glm::mat3x3 RotationMatrix(float rotation)
{
	float cosAngle{ cos(rotation) };
	float sinAngle{ sin(rotation) };
	return{
		cosAngle, -sinAngle, 0,
		sinAngle, cosAngle,0,
		0,0,1
	};
}

glm::mat3x3 TransformationMatrix(const glm::vec2& pos, const glm::vec2& scale, float rotation)
{
	float cosAngle{ cos(rotation) };
	float sinAngle{ sin(rotation) };
	return glm::mat3x3{
		scale.x * cosAngle, -sinAngle * scale.x, pos.x,
		scale.y * sinAngle, scale.y * cosAngle, pos.y,
		0,0,1
	};
}
