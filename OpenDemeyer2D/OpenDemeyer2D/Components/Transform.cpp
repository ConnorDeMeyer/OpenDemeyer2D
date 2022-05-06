#include "Transform.h"

#include "EngineFiles/GameObject.h"
#include "imgui.h"
#include "UtilityFiles/Dictionary.h"


void Transform::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<glm::vec2>("position", offsetof(Transform, m_LocalPosition));
	binder.Add<glm::vec2>("scale", offsetof(Transform, m_LocalScale));
	binder.Add<float>("rotation", offsetof(Transform, m_LocalRotation));
}

void Transform::Initialize()
{
	UpdateLocalChanges();
}

void Transform::RenderImGui()
{
	// Display Position
	glm::vec2 position = m_LocalPosition;
	ImGui::InputFloat2("Position", reinterpret_cast<float*>(&position));

	// Change Position
	if (position != m_LocalPosition)
		SetPosition(position);

	// Display scale
	glm::vec2 scale = m_LocalScale;
	ImGui::InputFloat2("Scale", reinterpret_cast<float*>(&scale));

	// Change scale
	const float EPSILON{ 0.0000001f };
	if (scale != m_LocalScale && fabsf(scale.x) > EPSILON && fabsf(scale.y) > EPSILON)
		SetScale(scale);

	// Display rotation
	float rotation = m_LocalRotation;
	ImGui::InputFloat("Rotation", &rotation, 1.f, 2.f);

	// Change rotation
	if (rotation != m_LocalRotation)
		SetRotation(rotation);

	// Display world transforms
	ImGui::Text("World Position: [%.1f,%.1f]", m_Position.x, m_Position.y);
	ImGui::Text("World Scale:    [%.1f,%.1f]", m_Scale.x, m_Scale.y);
	ImGui::Text("World Rotation: [%.1f]", m_Rotation);
}


void Transform::SetPosition(const glm::vec2& pos)
{
	glm::vec2 difference = pos - m_LocalPosition;
	Move(difference);
}


void Transform::SetScale(const glm::vec2& scale)
{
	glm::vec2 difference = scale / m_LocalScale;
	Scale(difference);
}

void Transform::SetRotation(float rotation)
{
	float difference = rotation - m_LocalRotation;

	Rotate(difference);
}

void Transform::Move(const glm::vec2& distance)
{
	m_LocalPosition += distance;
	UpdateLocalChanges();
}

void Transform::Scale(const glm::vec2& scalar)
{
	m_LocalScale *= scalar;
	UpdateLocalChanges();
}

void Transform::Rotate(float rotation)
{
	m_LocalRotation += rotation;
	UpdateLocalChanges();
}

void Transform::UpdateLocalChanges()
{
	m_localTransformation = TransformationMatrix(m_LocalPosition, m_LocalScale, m_LocalRotation);
	if (GameObject * pParent{ GetParent()->GetParent() }) {
		m_Transformation = m_localTransformation * pParent->GetTransform()->m_Transformation;
		m_Position = GetPosFromMat(m_Transformation);
		m_Scale = GetScaleFromMat(m_Transformation);
		m_Rotation = GetRotationFromMat(m_Transformation);
	}
	else
	{
		m_Transformation = m_localTransformation;
		m_Position = m_LocalPosition;
		m_Scale = m_LocalScale;
		m_Rotation = m_LocalRotation;
	}

	for (GameObject* child : GetParent()->GetChildren())
		child->GetTransform()->ApplyMatrix(m_Transformation);
}

void Transform::Scale(float scale) { Scale({ scale, scale }); }

void Transform::AddScale(const glm::vec2& scale){ Scale({ (m_Scale + scale) / m_Scale }); }

void Transform::AddScale(float scale){ AddScale({ scale,scale }); }

void Transform::ApplyMatrix(const glm::mat3x3& matrix)
{
	m_Transformation = m_localTransformation * matrix;
	m_Position = GetPosFromMat(m_Transformation);
	m_Scale = GetScaleFromMat(m_Transformation);
	m_Rotation = GetRotationFromMat(m_Transformation);

	for (GameObject* pObject : GetParent()->GetChildren())
	{
		pObject->GetTransform()->ApplyMatrix(m_Transformation);
	}
}

glm::vec2 GetPosFromMat(const glm::mat3x3& matrix)
{
	return { matrix[0][2], matrix[1][2] };
}

glm::vec2 GetScaleFromMat(const glm::mat3x3& matrix)
{
	const float SquareSum0 = (matrix[0][0] * matrix[0][0]) + (matrix[1][0] * matrix[1][0]);
	const float SquareSum1 = (matrix[0][1] * matrix[0][1]) + (matrix[1][1] * matrix[1][1]);

	return
	{	glm::sign(matrix[0][0]) * glm::sqrt(SquareSum0),
		glm::sign(matrix[1][1]) * glm::sqrt(SquareSum1) };
}

float GetRotationFromMat(const glm::mat3x3& matrix)
{
	constexpr float toRadian{ 1.f * 180.f / float(M_PI) };

	return atan2(matrix[1][0], matrix[1][1]) * toRadian;
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
	constexpr float inverse180{ 1.f / 180.f * float(M_PI) };

	float cosAngle{ cos(rotation * inverse180) };
	float sinAngle{ sin(rotation * inverse180) };
	return glm::mat3x3{
		scale.x * cosAngle, -sinAngle * scale.x, pos.x,
		scale.y * sinAngle, scale.y * cosAngle, pos.y,
		0,0,1
	};
}
