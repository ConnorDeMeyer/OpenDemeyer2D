#pragma once
#include <SDL.h>
#include <gl/glew.h>

/** Simple RAII container for the sdl texture*/
class Texture2D final
{
	friend class RenderManager;

public:
	//SDL_Texture* GetSDLTexture() const { return m_Texture; }

	explicit Texture2D(GLuint id, int width, int height)
		: m_Id{ id }, m_Width{ width }, m_Height{ height }{}
	~Texture2D(){ glDeleteTextures(1, &m_Id); }
	
	Texture2D(const Texture2D&) = delete;
	Texture2D(Texture2D&&) = delete;
	Texture2D& operator= (const Texture2D&) = delete;
	Texture2D& operator= (const Texture2D&&) = delete;

	GLuint GetId() { return m_Id; }
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

private:
	GLuint m_Id{};
	int m_Width{};
	int m_Height{};
};