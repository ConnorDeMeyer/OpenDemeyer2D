#pragma once
#include <gl/glew.h>

class RenderTarget final
{

	friend class RenderManager;

public:

	explicit RenderTarget(GLuint frameBuffer, GLuint renderedTexture, int width, int height)
		: m_FrameBuffer{ frameBuffer }, m_RenderedTexture{ renderedTexture }, m_width{ width }, m_height{ height }
		{}

	~RenderTarget()
	{
		glDeleteTextures(1, &m_RenderedTexture);
		glDeleteBuffers(1, &m_FrameBuffer);
	}
	
	RenderTarget(const RenderTarget&) = delete;
	RenderTarget(RenderTarget&&) = delete;
	RenderTarget& operator= (const RenderTarget&) = delete;
	RenderTarget& operator= (const RenderTarget&&) = delete;

	GLuint GetFrameBuffer() { return m_FrameBuffer; }
	GLuint GetRenderedTexture() { return m_RenderedTexture; }
	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }

private:

	GLuint m_FrameBuffer{};
	GLuint m_RenderedTexture{};
	int m_width{};
	int m_height{};

};

