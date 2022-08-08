#pragma once

#include "ShaderBase.h"
#include <glm/glm.hpp>
#include "GlArrayBuffer.h"
#include "GLVertexArrayObject.h"

class CircleShader final : protected ShaderBase
{
	friend class ShaderManager;

public:
	struct CircleData final
	{
		glm::mat3 transform;
		glm::vec4 color;

		CircleData() = default;
		~CircleData() = default;
		CircleData(const glm::mat3& transform, const glm::vec4& color);
		CircleData(const glm::vec2& position, float radius, const glm::vec4& color);
		CircleData(const CircleData&) = default;
		CircleData(CircleData&&) noexcept = default;

		CircleData& operator=(const CircleData&) = default;
		CircleData& operator=(CircleData&&) noexcept = default;
	};

	void SetCircles(const std::vector<CircleData>& circleData);
	void SetCircles(const CircleData* circleData, size_t size);
	void SetCircle(const CircleData& circleData);

	void Draw() const override;

private:

	CircleShader();

private:

	GlArrayBuffer<CircleData> m_Buffer{ 1, GLBufferUsage::DynamicDraw };
	GLVertexArrayObject m_VertexAttr{};

};


