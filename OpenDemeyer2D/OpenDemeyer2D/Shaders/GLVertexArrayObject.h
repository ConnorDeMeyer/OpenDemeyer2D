#pragma once
#include <EngineIO/Reflection.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

class GLVertexArrayObject final
{
private:

	static constexpr GLenum GetType(uint32_t id)
	{
		switch (id)
		{
		case class_id<int8_t>():		return GL_BYTE;
		case class_id<uint8_t>():		return GL_UNSIGNED_BYTE;
		case class_id<int16_t>():		return GL_SHORT;
		case class_id<uint16_t>():		return GL_UNSIGNED_SHORT;
		case class_id<int32_t>():		return GL_INT;
		case class_id<uint32_t>():		return GL_UNSIGNED_INT;
		case class_id<float>():		
		case class_id<glm::vec2>():	
		case class_id<glm::vec3>():	
		case class_id<glm::vec4>():		return GL_FLOAT;
		default: return 0;
		}
	}

public:

	GLVertexArrayObject()
	{
        glGenVertexArrays(1, &m_VertexArrayId);
	}

	~GLVertexArrayObject()
	{
        if (m_VertexArrayId)
            glDeleteVertexArrays(1, &m_VertexArrayId);
	}

	GLVertexArrayObject(const GLVertexArrayObject&) = delete;
	GLVertexArrayObject& operator=(const GLVertexArrayObject&) = delete;

	GLVertexArrayObject(GLVertexArrayObject&& other) noexcept
        :m_VertexArrayId(other.m_VertexArrayId)
	{
        other.m_VertexArrayId = 0;
	}
	GLVertexArrayObject& operator=(GLVertexArrayObject&& other) noexcept
	{
        std::swap(m_VertexArrayId, other.m_VertexArrayId);
        return *this;
	}

	template <typename T>
	void ConfigureVertexAttribute(GLint index, size_t offset, size_t stride, bool normalized = false) const
	{
		// OpenGL only allows types of size up to 16 bytes (4 of 4 bytes)
		// If using a matrix please please do the following:
		// - inside of the shader:
		//   layout(location = 0) in mat3 transform;
		//   layout(location = 3) in vec4 color;
		// - inside of code:
		//	 m_Buffer.ConfigureVertexArray<glm::vec3>(0, 0);
		//	 m_Buffer.ConfigureVertexArray<glm::vec3>(1, 12));
		//	 m_Buffer.ConfigureVertexArray<glm::vec3>(2, 24));
		//   m_Buffer.ConfigureVertexArray<glm::vec4>(3, 36);
		static_assert(sizeof(T) <= 16);
		static_assert(sizeof(T) % 4 == 0 || sizeof(T) < 4);
		static_assert(GetType(class_id<T>()));

		glBindVertexArray(m_VertexArrayId);
		glVertexAttribPointer(index, sizeof(T) / 4, GetType(class_id<T>()), normalized, GLsizei(stride), (void*)offset);
		glEnableVertexAttribArray(index);
	}

    void SetActive() const { glBindVertexArray(m_VertexArrayId); }
    GLuint GetBufferId() const { return m_VertexArrayId; }

private:

	GLuint m_VertexArrayId{};

};
