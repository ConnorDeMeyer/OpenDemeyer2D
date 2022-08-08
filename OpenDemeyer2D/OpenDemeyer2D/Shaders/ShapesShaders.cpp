#include "pch.h"
#include "ShapesShaders.h"

CircleShader::CircleData::CircleData(const glm::mat3& _transform, const glm::vec4& _color)
	: transform(_transform)
	, color(_color)
{}

CircleShader::CircleData::CircleData(const glm::vec2& position, float radius, const glm::vec4& color)
	: transform(
		radius,	0,		position.x,
		0,		radius,	position.y,
		0,		0,		1)
	, color(color)
{}

void CircleShader::SetCircles(const std::vector<CircleData>& circleData)
{
	SetCircles(circleData.data(), circleData.size());
}

void CircleShader::SetCircles(const CircleData* circleData, size_t size)
{
	m_Buffer.SetData(circleData, size);
}

void CircleShader::SetCircle(const CircleData& circleData)
{
	m_Buffer.SetData(&circleData, 1);
}

void CircleShader::Draw() const
{
	SetActive();
	m_Buffer.DrawArray(GLBufferDrawMode::Points);
}

CircleShader::CircleShader()
	: ShaderBase(
		std::filesystem::path("../Resources/Shaders/Circle.fs"), 
		std::filesystem::path("../Resources/Shaders/SimpleSquare.vs"), 
		std::filesystem::path("../Resources/Shaders/SimpleSquare.gs"))
{
	m_VertexAttr.ConfigureVertexAttribute<glm::vec3>(0, 0, sizeof(CircleData));
	m_VertexAttr.ConfigureVertexAttribute<glm::vec3>(1, 12, sizeof(CircleData));
	m_VertexAttr.ConfigureVertexAttribute<glm::vec3>(2, 24, sizeof(CircleData));
	m_VertexAttr.ConfigureVertexAttribute<glm::vec4>(3, offsetof(CircleData, CircleData::color), sizeof(CircleData));
}

