#include "pch.h"
#include "ShaderBase.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "string_view"


ShaderBase::ShaderBase(const std::filesystem::path& fragmentPath, const std::filesystem::path& vertexPath,
	const std::filesystem::path& geometryPath)
		: ShaderBase{
			fragmentPath.empty() ? nullptr : GetShaderCode(fragmentPath).c_str(),
			vertexPath.empty()   ? nullptr : GetShaderCode(vertexPath).c_str(),
			geometryPath.empty() ? nullptr : GetShaderCode(geometryPath).c_str()
		}
{}

ShaderBase::ShaderBase(const char* fragmentCode, const char* vertexCode, const char* geometryCode)
{
	GLint fragmentShader{}, vertexShader{}, geometryShader{};

	if (fragmentCode)
		fragmentShader = CompileShader(fragmentCode, ShaderType::FragmentShader);
	if (vertexCode)
		vertexShader = CompileShader(vertexCode, ShaderType::VertexShader);
	if (geometryCode)
		geometryShader = CompileShader(geometryCode, ShaderType::GeometryShader);

	m_Id = glCreateProgram();
	if (fragmentShader)
		glAttachShader(m_Id, fragmentShader);
	if (vertexShader)
		glAttachShader(m_Id, vertexShader);
	if (geometryShader)
		glAttachShader(m_Id, geometryShader);

	glLinkProgram(m_Id);

	GLint success;
	char infoLog[512]{};

	glGetProgramiv(m_Id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_Id, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	if (fragmentShader)
		glDeleteShader(fragmentShader);
	if (vertexShader)
		glDeleteShader(vertexShader);
	if (geometryShader)
		glDeleteShader(geometryShader);
}

ShaderBase::~ShaderBase()
{
	glDeleteProgram(m_Id);
}

void ShaderBase::SetActive() const
{
	glUseProgram(m_Id);
}

GLint ShaderBase::GetUniformLocation(const char* name) const
{
	return glGetUniformLocation(m_Id, name);
}

GLint ShaderBase::GetUniformLocation(const std::string& name) const
{
	return GetUniformLocation(name.c_str());
}

void PrintShaderType(ShaderType type)
{
	switch (type)
	{
	case ShaderType::VertexShader:
		std::cout << "VERTEXSHADER";
		break;
	case ShaderType::FragmentShader:
		std::cout << "FRAGMENTSHADER";
		break;
	case ShaderType::GeometryShader:
		std::cout << "GEOMETRYSHADER";
		break;
	case ShaderType::ComputeShader:
		std::cout << "COMPUTESHADER";
		break;
	case ShaderType::TessControlShader:
		std::cout << "TESSCONTROLSHADER";
		break;
	case ShaderType::TessEvaluationShader:
		std::cout << "TESSEVALUATIONSHADER";
		break;
	}
}

GLint CompileShader(const char* code, ShaderType type)
{
	if (!code)
		return 0;

#if _DEBUG
	GLint success{};
	char infoLog[4096]{};
#else
	GLint success{};
	char infoLog[4]{};
#endif


	GLint shader = glCreateShader(GLenum(type));
	glShaderSource(shader, 1, &code, nullptr);
	glCompileShader(shader);
	// print compile errors if any
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 4096, nullptr, infoLog);
		std::cout << "ERROR::SHADER::";
		PrintShaderType(type);
		std::cout << "::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shader;
}

std::string GetShaderCode(const std::filesystem::path& path)
{
	std::ifstream ifStream;
	std::stringstream sstream;

	ifStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		ifStream.open(path);
		sstream << ifStream.rdbuf();
		// close file handlers
		ifStream.close();
		// convert stream into string
		return sstream.str();
	}
	catch (std::ifstream::failure&)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	return {};
}

void SetUniformValue(GLint location, bool value)
{
	glUniform1i(location, int(value));
}

void SetUniformValue(GLint location, int value)
{
	glUniform1i(location, value);
}

void SetUniformValue(GLint location, uint32_t value)
{
	glUniform1ui(location, value);
}

void SetUniformValue(GLint location, float value)
{
	glUniform1f(location, value);
}

void SetUniformValue(GLint location, const glm::vec2& value)
{
	glUniform2fv(location, 1, &value.x);
}

void SetUniformValue(GLint location, const glm::vec3& value)
{
	glUniform3fv(location, 1, &value.x);
}

void SetUniformValue(GLint location, const glm::vec4& value)
{
	glUniform4fv(location, 1, &value.x);
}

void SetUniformValue(GLint location, const glm::ivec2& value)
{
	glUniform2iv(location, 1, &value.x);
}

void SetUniformValue(GLint location, const glm::ivec3& value)
{
	glUniform3iv(location, 1, &value.x);
}

void SetUniformValue(GLint location, const glm::ivec4& value)
{
	glUniform4iv(location, 1, &value.x);
}

void SetUniformValue(GLint location, const glm::uvec2& value)
{
	glUniform2uiv(location, 1, &value.x);
}

void SetUniformValue(GLint location, const glm::uvec3& value)
{
	glUniform3uiv(location, 1, &value.x);
}

void SetUniformValue(GLint location, const glm::uvec4& value)
{
	glUniform4uiv(location, 1, &value.x);
}

void SetUniformValue(GLint location, const glm::mat2& value)
{
	glUniformMatrix2fv(location, 1, false, &value[0][0]);
}

void SetUniformValue(GLint location, const glm::mat3& value)
{
	glUniformMatrix3fv(location, 1, false, &value[0][0]);
}

void SetUniformValue(GLint location, const glm::mat4& value)
{
	glUniformMatrix4fv(location, 1, false, &value[0][0]);
}

void SetUniformValue(GLint location, const glm::mat2x3& value)
{
	glUniformMatrix2x3fv(location, 1, false, &value[0][0]);
}

void SetUniformValue(GLint location, const glm::mat2x4& value)
{
	glUniformMatrix2x4fv(location, 1, false, &value[0][0]);
}

void SetUniformValue(GLint location, const glm::mat3x2& value)
{
	glUniformMatrix3x2fv(location, 1, false, &value[0][0]);
}

void SetUniformValue(GLint location, const glm::mat3x4& value)
{
	glUniformMatrix3x4fv(location, 1, false, &value[0][0]);
}

void SetUniformValue(GLint location, const glm::mat4x2& value)
{
	glUniformMatrix4x2fv(location, 1, false, &value[0][0]);
}

void SetUniformValue(GLint location, const glm::mat4x3& value)
{
	glUniformMatrix4x3fv(location, 1, false, &value[0][0]);
}

ShaderInstance::ShaderParameter::ShaderParameter(const char* name, const ShaderBase& shader)
	: m_Location(shader.GetUniformLocation(name))
{}

ShaderInstance::ShaderInstance(ShaderBase& shader)
	: m_OriginalShader(shader)
{}

ShaderInstance::ShaderInstance(const ShaderInstance& other)
	: m_OriginalShader(other.m_OriginalShader)
{
	m_Parameters.reserve(other.m_Parameters.size());
	for (auto& param : other.m_Parameters)
	{
		m_Parameters.emplace_back(param->CreateCopy());
	}
}

ShaderInstance::ShaderInstance(ShaderInstance&& other) noexcept
	: m_OriginalShader(other.m_OriginalShader)
	, m_Parameters(std::move(other.m_Parameters))
{}

void ShaderBase::SetBool(const char* name, bool value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetInt(const char* name, int value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetUInt(const char* name, uint32_t value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetFloat(const char* name, float value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetVec2(const char* name, const glm::vec2& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetVec3(const char* name, const glm::vec3& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetVec4(const char* name, const glm::vec4& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetIVec2(const char* name, const glm::ivec2& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetIVec3(const char* name, const glm::ivec3& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetIVec4(const char* name, const glm::ivec4& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetUVec2(const char* name, const glm::uvec2& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetUVec3(const char* name, const glm::uvec3& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetUVec4(const char* name, const glm::uvec4& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetMat2(const char* name, const glm::mat2& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetMat3(const char* name, const glm::mat3& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetMat4(const char* name, const glm::mat4& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetMat2x3(const char* name, const glm::mat2x3& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetMat2x4(const char* name, const glm::mat2x4& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetMat3x2(const char* name, const glm::mat3x2& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetMat3x4(const char* name, const glm::mat3x4& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetMat4x2(const char* name, const glm::mat4x2& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetMat4x3(const char* name, const glm::mat4x3& value) const
{
	SetUniformValue(glGetUniformLocation(m_Id, name), value);
}

void ShaderBase::SetBool(const std::string& name, bool value) const
{
	SetBool(name.c_str(), value);
}

void ShaderBase::SetInt(const std::string& name, int value) const
{
	SetInt(name.c_str(), value);
}

void ShaderBase::SetUInt(const std::string& name, uint32_t value) const
{
	SetUInt(name.c_str(), value);
}

void ShaderBase::SetFloat(const std::string& name, float value) const
{
	SetFloat(name.c_str(), value);
}

void ShaderBase::SetVec2(const std::string& name, const glm::vec2& value) const
{
	SetVec2(name.c_str(), value);
}

void ShaderBase::SetVec3(const std::string& name, const glm::vec3& value) const
{
	SetVec3(name.c_str(), value);
}

void ShaderBase::SetVec4(const std::string& name, const glm::vec4& value) const
{
	SetVec4(name.c_str(), value);
}

void ShaderBase::SetIVec2(const std::string& name, const glm::ivec2& value) const
{
	SetIVec2(name.c_str(), value);
}

void ShaderBase::SetIVec3(const std::string& name, const glm::ivec3& value) const
{
	SetIVec3(name.c_str(), value);
}

void ShaderBase::SetIVec4(const std::string& name, const glm::ivec4& value) const
{
	SetIVec4(name.c_str(), value);
}

void ShaderBase::SetUVec2(const std::string& name, const glm::uvec2& value) const
{
	SetUVec2(name.c_str(), value);
}

void ShaderBase::SetUVec3(const std::string& name, const glm::uvec3& value) const
{
	SetUVec3(name.c_str(), value);
}

void ShaderBase::SetUVec4(const std::string& name, const glm::uvec4& value) const
{
	SetUVec4(name.c_str(), value);
}

void ShaderBase::SetMat2(const std::string& name, const glm::mat2& value) const
{
	SetMat2(name.c_str(), value);
}

void ShaderBase::SetMat3(const std::string& name, const glm::mat3& value) const
{
	SetMat3(name.c_str(), value);
}

void ShaderBase::SetMat4(const std::string& name, const glm::mat4& value) const
{
	SetMat4(name.c_str(), value);
}

void ShaderBase::SetMat2x3(const std::string& name, const glm::mat2x3& value) const
{
	SetMat2x3(name.c_str(), value);
}

void ShaderBase::SetMat2x4(const std::string& name, const glm::mat2x4& value) const
{
	SetMat2x4(name.c_str(), value);
}

void ShaderBase::SetMat3x2(const std::string& name, const glm::mat3x2& value) const
{
	SetMat3x2(name.c_str(), value);
}

void ShaderBase::SetMat3x4(const std::string& name, const glm::mat3x4& value) const
{
	SetMat3x4(name.c_str(), value);
}

void ShaderBase::SetMat4x2(const std::string& name, const glm::mat4x2& value) const
{
	SetMat4x2(name.c_str(), value);
}

void ShaderBase::SetMat4x3(const std::string& name, const glm::mat4x3& value) const
{
	SetMat4x3(name.c_str(), value);
}
