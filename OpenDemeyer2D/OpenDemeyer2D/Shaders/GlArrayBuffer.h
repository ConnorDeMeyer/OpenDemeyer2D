#pragma once

#include <gl/glew.h>
#include "EngineIO/Reflection.h"
#include <unordered_map>

enum class GLBufferUsage
{
    /**
     * STREAM: The data store contents will be modified once and used at most a few times.\n
     * DRAW: The data store contents are modified by the application, and used as the source for GL drawing and image specification commands.
     */
	StreamDraw  = GL_STREAM_DRAW,

    /**
     * STREAM: The data store contents will be modified once and used at most a few times.\n
     * READ: The data store contents are modified by reading data from the GL, and used to return that data when queried by the application.
     */
	StreamRead  = GL_STREAM_READ,

    /**
     * STREAM: The data store contents will be modified once and used at most a few times.\n
     * COPY: The data store contents are modified by reading data from the GL, and used as the source for GL drawing and image specification commands.
     */
    StreamCopy  = GL_STREAM_COPY,

    /**
     * STATIC: The data store contents will be modified once and used many times.\n
     * DRAW: The data store contents are modified by the application, and used as the source for GL drawing and image specification commands.
     */
    StaticDraw  = GL_STATIC_DRAW,

    /**
     * STATIC: The data store contents will be modified once and used many times.\n
     * READ: The data store contents are modified by reading data from the GL, and used to return that data when queried by the application.
     */
    StaticRead  = GL_STATIC_READ,

    /**
     * STATIC: The data store contents will be modified once and used many times.\n
     * COPY: The data store contents are modified by reading data from the GL, and used as the source for GL drawing and image specification commands.
     */
    StaticCopy  = GL_STATIC_COPY,

    /**
     * DYNAMIC: The data store contents will be modified repeatedly and used many times.\n
     * DRAW: The data store contents are modified by the application, and used as the source for GL drawing and image specification commands.
     */
    DynamicDraw = GL_DYNAMIC_DRAW,

    /**
     * DYNAMIC: The data store contents will be modified repeatedly and used many times.\n
     * READ: The data store contents are modified by reading data from the GL, and used to return that data when queried by the application.
     */
    DynamicRead = GL_DYNAMIC_READ,

    /**
     * DYNAMIC: The data store contents will be modified repeatedly and used many times.\n
     * COPY: The data store contents are modified by reading data from the GL, and used as the source for GL drawing and image specification commands.
     */
    DynamicCopy = GL_DYNAMIC_COPY
};

enum class GLBufferDrawMode
{
	Points                  = GL_POINTS,
    LineStrip               = GL_LINE_STRIP,
    LineLoop                = GL_LINE_LOOP,
    Lines                   = GL_LINES,
    LineStripAdjacency      = GL_LINE_STRIP_ADJACENCY,
    LineAdjacency           = GL_LINE_STRIP_ADJACENCY,
    TriangleStrip           = GL_TRIANGLE_STRIP,
    TriangleFan             = GL_TRIANGLE_FAN,
    Triangles               = GL_TRIANGLES,
    TriangleStripAdjacency  = GL_TRIANGLE_STRIP_ADJACENCY,
    TriangleAdjacency       = GL_TRIANGLES_ADJACENCY,
    Patches                 = GL_PATCHES,
};

template <typename Data>
class GlArrayBuffer final
{

public:

    GlArrayBuffer(size_t capacity = 0, GLBufferUsage usage = GLBufferUsage::DynamicDraw)
	    : m_Capacity(capacity)
		, m_Usage(usage)
    {
        glGenBuffers(1, &m_BufferId);

        glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Data) * capacity, nullptr, GLenum(usage));
    }
	~GlArrayBuffer()
	{
        if (m_BufferId)
			glDeleteBuffers(1, &m_BufferId);
	}

    GlArrayBuffer(const GlArrayBuffer&) = delete;
    GlArrayBuffer& operator=(const GlArrayBuffer&) = delete;

    GlArrayBuffer(GlArrayBuffer&& other) noexcept
	    : m_BufferId(other.m_BufferId)
		, m_Capacity(other.m_Capacity)
		, m_Usage(other.m_Usage)
    {
        other.m_BufferId = 0;
        other.m_Capacity = 0;
    }
    GlArrayBuffer& operator=(GlArrayBuffer&& other) noexcept
    {
        std::swap(m_BufferId, other.m_BufferId);
        std::swap(m_Capacity, other.m_Capacity);
        m_Usage = other.m_Usage;
        
        return *this;
    }

    void SetData(const Data* data, size_t size)
    {
        if (size > m_Capacity)
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(Data) * size, data, GLenum(m_Usage));
            m_Capacity = size;
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
            auto pBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
            std::memcpy(pBuffer, data, size * sizeof(Data));
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        m_Size = size;
    }

    void SetSubData(const Data* data, size_t offset, size_t size)
    {
	    if (offset + size > m_Capacity)
	    {
            // TODO copy previous data
            glBufferData(GL_ARRAY_BUFFER, sizeof(Data) * (size + offset), nullptr, GLenum(m_Usage));
            m_Capacity = size + offset;
	    }

        m_Size = std::max(size + offset, m_Size);

        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }

    void DrawArray(GLBufferDrawMode mode) const
    {
        SetActive();
        glDrawArrays(GLenum(mode), 0, GLsizei(m_Size));
    }

    void DrawSubArray(GLBufferDrawMode mode, size_t offset, size_t size) const
    {
        SetActive();
        glDrawArrays(GLenum(mode), offset, std::min(size, m_Size - offset));
    }

    GLuint GetBufferId() const { return m_BufferId; }
    void SetActive() const { glBindBuffer(GL_ARRAY_BUFFER, m_BufferId); }

private:

    void UpdateBufferSize(size_t size)
    {
	    if (size > m_Capacity)
	    {
            glBufferData(GL_ARRAY_BUFFER, sizeof(Data) * size, nullptr, GLenum(m_Usage));
	    }
    }

private:

	GLuint m_BufferId{};
    size_t m_Capacity{};
    size_t m_Size{};
    GLBufferUsage m_Usage{};

};

