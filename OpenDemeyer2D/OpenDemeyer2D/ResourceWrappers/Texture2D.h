#pragma once
#include <SDL.h>
#include <gl/glew.h>
#include <string>
#include <filesystem>

/** Simple RAII container for the sdl texture*/
class Texture2D final
{
	friend class RenderManager;
	friend class ResourceManager;

public:
	//SDL_Texture* GetSDLTexture() const { return m_Texture; }

	explicit Texture2D(GLuint id, int width, int height)
		: m_Id{ id }, m_Width{ width }, m_Height{ height }{}
	~Texture2D(){ glDeleteTextures(1, &m_Id); }
	
	Texture2D(const Texture2D&) = delete;
	Texture2D(Texture2D&&) = delete;
	Texture2D& operator= (const Texture2D&) = delete;
	Texture2D& operator= (const Texture2D&&) = delete;

	GLuint GetId() const { return m_Id; }
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }

	const std::filesystem::path& GetFilePath() const { return m_sourceFile; }

private:
	GLuint m_Id{};
	int m_Width{};
	int m_Height{};

	std::filesystem::path m_sourceFile{};
};