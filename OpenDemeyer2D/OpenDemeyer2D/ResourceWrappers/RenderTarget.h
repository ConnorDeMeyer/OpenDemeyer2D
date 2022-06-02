#pragma once
#include <gl/glew.h>

class RenderTarget final
{

	friend class RenderManager;

public:

	RenderTarget() = default;

	explicit RenderTarget(GLuint frameBuffer, GLuint renderedTexture, int width, int height)
		: m_FrameBuffer{ frameBuffer }, m_RenderedTexture{ renderedTexture }, m_width{ width }, m_height{ height }
		{}

	~RenderTarget()
	{
		glDeleteTextures(1, &m_RenderedTexture);
		glDeleteBuffers(1, &m_FrameBuffer);
	}
	
	RenderTarget(const RenderTarget&) = delete;
	RenderTarget& operator= (const RenderTarget&) = delete;

	RenderTarget(RenderTarget&& other) noexcept
		: m_FrameBuffer{ other.m_FrameBuffer }
		, m_RenderedTexture{ other.m_RenderedTexture }
		, m_width{ other.m_width }
		, m_height{ other.m_height }
	{
		other.m_FrameBuffer = 0;
		other.m_RenderedTexture = 0;
		other.m_width = 0;
		other.m_height = 0;
	}

	RenderTarget& operator= (RenderTarget&& other) noexcept
	{
		m_FrameBuffer = other.m_FrameBuffer;
		other.m_FrameBuffer = 0;
		m_RenderedTexture = other.m_RenderedTexture;
		other.m_RenderedTexture = 0;
		m_width = other.m_width;
		other.m_width = 0;
		m_height = other.m_height;
		other.m_height = 0;
		return *this;
	}

	GLuint GetFrameBuffer() { return m_FrameBuffer; }
	GLuint GetRenderedTexture() { return m_RenderedTexture; }
	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }

	inline bool IsValid() { return m_FrameBuffer; }
	inline operator bool() { return IsValid(); }

private:

	GLuint m_FrameBuffer{};
	GLuint m_RenderedTexture{};
	int m_width{};
	int m_height{};

};

