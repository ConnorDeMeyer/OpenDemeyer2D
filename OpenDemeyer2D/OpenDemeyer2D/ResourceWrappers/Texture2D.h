#pragma once
#include <gl/glew.h>
#include <filesystem>

/** Simple RAII container for the sdl texture*/
class Texture2D final
{
	friend class RenderManager;
	friend class ResourceManager;

public:
	//SDL_Texture* GetSDLTexture() const { return m_Texture; }
	Texture2D() = default;

	Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D&) = delete;

	Texture2D(Texture2D&& other) noexcept
		: m_Id{ other.m_Id }
		, m_Width{ other.m_Width }
		, m_Height{ other.m_Height }
		, m_sourceFile{ std::move(other.m_sourceFile) }
	{
		other.m_Id = 0;
		other.m_Height = 0;
		other.m_Width = 0;
	}

	Texture2D& operator=(Texture2D&& other) noexcept
	{
		m_Id = other.m_Id;
		other.m_Id = 0;
		m_Width = other.m_Width;
		other.m_Width = 0;
		m_Height = other.m_Height;
		other.m_Height = 0;
		m_sourceFile = std::move(other.m_sourceFile);
		return *this;
	}

	explicit Texture2D(GLuint id, int width, int height)
		: m_Id{ id }, m_Width{ width }, m_Height{ height }{}
	~Texture2D(){ glDeleteTextures(1, &m_Id); }

	GLuint GetId() const { return m_Id; }
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }

	const std::filesystem::path& GetFilePath() const { return m_sourceFile; }

	inline bool IsValid() { return m_Id; }
	inline operator bool() { return IsValid(); }


private:
	GLuint m_Id{};
	int m_Width{};
	int m_Height{};

	std::filesystem::path m_sourceFile{};
};