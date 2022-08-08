#pragma once

#include <GL/glew.h>
#include <filesystem>
#include <glm/glm.hpp>

/**
 * layout (std140) uniform Globals
 * {\n
 *	mat3 view	// 48\n
 *	mat3 invView	// 96\n
 *	float time	// 100\n
 *	float deltaTime	// 104\n
 * }
 */
struct GLSL_GlobalVariables
{
	glm::mat3x4 view;
	glm::mat3x4 invView;
	float time;
	float deltaTime;
};
static_assert(sizeof(GLSL_GlobalVariables) == 104);

enum class ShaderType : GLenum
{
	VertexShader			= GL_VERTEX_SHADER,
	FragmentShader			= GL_FRAGMENT_SHADER,
	GeometryShader			= GL_GEOMETRY_SHADER,
	ComputeShader			= GL_COMPUTE_SHADER,
	TessControlShader		= GL_TESS_CONTROL_SHADER,
	TessEvaluationShader	= GL_TESS_EVALUATION_SHADER,
};

class ShaderBase
{
public:

	ShaderBase(const std::filesystem::path& fragmentPath = {}, const std::filesystem::path& vertexPath = {}, const std::filesystem::path& geometryPath = {});
	ShaderBase(const char* fragmentCode = {}, const char* vertexCode = {}, const char* geometryCode = {});
	virtual ~ShaderBase();

	ShaderBase(const ShaderBase&)				= delete;
	ShaderBase(ShaderBase&&)					= delete;
	ShaderBase& operator=(const ShaderBase&)	= delete;
	ShaderBase& operator=(ShaderBase&&)			= delete;

public:

	GLint GetId() const { return m_Id; }

	void SetActive() const;

	virtual void Draw() const = 0;

	GLint GetUniformLocation(const char* name)							const;
	GLint GetUniformLocation(const std::string& name)					const;

	void SetBool(	const char* name, bool value)						const;
	void SetInt(	const char* name, int value)						const;
	void SetUInt(	const char* name, uint32_t value)					const;
	void SetFloat(	const char* name, float value)						const;
	void SetVec2(	const char* name, const glm::vec2& value)			const;
	void SetVec3(	const char* name, const glm::vec3& value)			const;
	void SetVec4(	const char* name, const glm::vec4& value)			const;
	void SetIVec2(	const char* name, const glm::ivec2& value)			const;
	void SetIVec3(	const char* name, const glm::ivec3& value)			const;
	void SetIVec4(	const char* name, const glm::ivec4& value)			const;
	void SetUVec2(	const char* name, const glm::uvec2& value)			const;
	void SetUVec3(	const char* name, const glm::uvec3& value)			const;
	void SetUVec4(	const char* name, const glm::uvec4& value)			const;
	void SetMat2(	const char* name, const glm::mat2& value)			const;
	void SetMat3(	const char* name, const glm::mat3& value)			const;
	void SetMat4(	const char* name, const glm::mat4& value)			const;
	void SetMat2x3(	const char* name, const glm::mat2x3& value)			const;
	void SetMat2x4(	const char* name, const glm::mat2x4& value)			const;
	void SetMat3x2(	const char* name, const glm::mat3x2& value)			const;
	void SetMat3x4(	const char* name, const glm::mat3x4& value)			const;
	void SetMat4x2(	const char* name, const glm::mat4x2& value)			const;
	void SetMat4x3(	const char* name, const glm::mat4x3& value)			const;

	void SetBool(	const std::string& name, bool value)				const;
	void SetInt(	const std::string& name, int value)					const;
	void SetUInt(	const std::string& name, uint32_t value)			const;
	void SetFloat(	const std::string& name, float value)				const;
	void SetVec2(	const std::string& name, const glm::vec2& value)	const;
	void SetVec3(	const std::string& name, const glm::vec3& value)	const;
	void SetVec4(	const std::string& name, const glm::vec4& value)	const;
	void SetIVec2(	const std::string& name, const glm::ivec2& value)	const;
	void SetIVec3(	const std::string& name, const glm::ivec3& value)	const;
	void SetIVec4(	const std::string& name, const glm::ivec4& value)	const;
	void SetUVec2(	const std::string& name, const glm::uvec2& value)	const;
	void SetUVec3(	const std::string& name, const glm::uvec3& value)	const;
	void SetUVec4(	const std::string& name, const glm::uvec4& value)	const;
	void SetMat2(	const std::string& name, const glm::mat2& value)	const;
	void SetMat3(	const std::string& name, const glm::mat3& value)	const;
	void SetMat4(	const std::string& name, const glm::mat4& value)	const;
	void SetMat2x3(	const std::string& name, const glm::mat2x3& value)	const;
	void SetMat2x4(	const std::string& name, const glm::mat2x4& value)	const;
	void SetMat3x2(	const std::string& name, const glm::mat3x2& value)	const;
	void SetMat3x4(	const std::string& name, const glm::mat3x4& value)	const;
	void SetMat4x2(	const std::string& name, const glm::mat4x2& value)	const;
	void SetMat4x3(	const std::string& name, const glm::mat4x3& value)	const;

protected:



private:

	GLint m_Id{};

};

[[nodiscard]] GLint CompileShader(const char* code, ShaderType type);
[[nodiscard]] std::string GetShaderCode(const std::filesystem::path& path);

void SetUniformValue(GLint location, bool value);
void SetUniformValue(GLint location, int value);
void SetUniformValue(GLint location, uint32_t value);
void SetUniformValue(GLint location, float value);
void SetUniformValue(GLint location, const glm::vec2& value);
void SetUniformValue(GLint location, const glm::vec3& value);
void SetUniformValue(GLint location, const glm::vec4& value);
void SetUniformValue(GLint location, const glm::ivec2& value);
void SetUniformValue(GLint location, const glm::ivec3& value);
void SetUniformValue(GLint location, const glm::ivec4& value);
void SetUniformValue(GLint location, const glm::uvec2& value);
void SetUniformValue(GLint location, const glm::uvec3& value);
void SetUniformValue(GLint location, const glm::uvec4& value);
void SetUniformValue(GLint location, const glm::mat2& value);
void SetUniformValue(GLint location, const glm::mat3& value);
void SetUniformValue(GLint location, const glm::mat4& value);
void SetUniformValue(GLint location, const glm::mat2x3& value);
void SetUniformValue(GLint location, const glm::mat2x4& value);
void SetUniformValue(GLint location, const glm::mat3x2& value);
void SetUniformValue(GLint location, const glm::mat3x4& value);
void SetUniformValue(GLint location, const glm::mat4x2& value);
void SetUniformValue(GLint location, const glm::mat4x3& value);

class ShaderInstance final
{
private:

	class ShaderParameter
	{
	public:
		ShaderParameter(const char* name, const ShaderBase& shader);
		virtual ~ShaderParameter() = default;

		virtual void SetParameter() const = 0;
		virtual std::unique_ptr<ShaderParameter> CreateCopy() const = 0;

		bool IsValid() const { return m_Location; }
		GLint GetLocation() const { return m_Location; }
	private:
		GLint m_Location{};
	};

	template <typename T>
	class ShaderParameterValue : public ShaderParameter
	{
	public:
		ShaderParameterValue(const char* name, const ShaderBase& shader, const T& value)
			: ShaderParameter(name, shader)
			, m_Value(value)
		{}
		virtual ~ShaderParameterValue() = default;

		void SetParameter() const override
		{
			SetUniformValue(GetLocation(), m_Value);
		}

		std::unique_ptr<ShaderParameter> CreateCopy() const override
		{
			return std::make_unique<ShaderParameterValue<T>>(*this);
		}

	private:
		T m_Value;
	};

public:

	ShaderInstance(ShaderBase& shader);
	~ShaderInstance() = default;

	ShaderInstance(const ShaderInstance& other);
	ShaderInstance(ShaderInstance&& other) noexcept;
	ShaderInstance& operator=(const ShaderInstance& other) = delete;
	ShaderInstance& operator=(ShaderInstance&& other) = delete;

	template <typename T>
	void SetParameter(const char* name, const T& value)
	{
		ShaderParameterValue<T> parameterValue(name, m_OriginalShader, value);

		if (parameterValue.IsValid())
		{
			GLint location = parameterValue.GetLocation();
			for (auto& parameter : m_Parameters)
			{
				// if the parameter is already in there
				if (parameter->GetLocation() == location)
				{
					*parameter = parameterValue;
					return;
				}
			}

			m_Parameters.emplace_back(new ShaderParameterValue<T>(parameterValue));
		}
	}

	void Activate() const
	{
		for (auto& param : m_Parameters)
			param->SetParameter();
	}

private:
	ShaderBase& m_OriginalShader;

	std::vector<std::unique_ptr<ShaderParameter>> m_Parameters;
};

/**
 * Shader instance reference to be put inside of shader vertex data
 * Will always have a size of 8 bytes.
 */
class ShaderInstanceRef final
{
public:

	ShaderInstanceRef(ShaderInstance& instance)
		: m_ShaderInstance(&instance)
	{}

	ShaderInstance* Get() { return m_ShaderInstance; }
	const ShaderInstance* Get() const { return m_ShaderInstance; }

private:
	ShaderInstance* m_ShaderInstance;
	
#if (_WIN32 && !_WIN64) || (__x86_64__ && !__ppc64__)
	uint32_t m_Filler{};
#endif

};

static_assert(sizeof(ShaderInstanceRef) == 8);