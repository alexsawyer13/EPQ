#include <Graphics/Shader.h>
#include <Core/Assets.h>

#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>

Shader::Shader()
{

}

Shader::~Shader()
{
	Destroy();
}

void Shader::Create(const std::string &vertexPath, const std::string &fragmentPath)
{
	Destroy();

	std::string vertexSource = ReadAsset("shaders/" + vertexPath);
	std::string fragmentSource = ReadAsset("shaders/" + fragmentPath);

	GLuint vertexShader = LoadShader(vertexSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = LoadShader(fragmentSource, GL_FRAGMENT_SHADER);

	m_Handle = LinkProgram(vertexShader, fragmentShader);

	FreeShader(vertexShader);
	FreeShader(fragmentShader);
}

void Shader::Destroy()
{
	if (m_Handle)
	{
		glDeleteProgram(m_Handle);
		m_UniformCache.clear();
	}
}

GLuint Shader::LoadShader(const std::string &source, GLenum type)
{
	GLuint shader = glCreateShader(type);

	const char *src = source.c_str();

	glShaderSource(shader, 1, &src, 0);

	glCompileShader(shader);

	GLint temp;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &temp);
	if (!temp)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &temp);
		char *log = new char[temp];
		glGetShaderInfoLog(shader, temp, nullptr, log);
		spdlog::critical("Failed to compile shader: {}\n{}", source, log);
		delete[] log;
	}

	return shader;
}

GLuint Shader::LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint temp;
	glGetProgramiv(program, GL_LINK_STATUS, &temp);
	if (!temp)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &temp);
		char *log = new char[temp];
		glGetProgramInfoLog(program, temp, nullptr, log);
		spdlog::critical("Failed to link program\n{}", log);
		delete[] log;
	}

	return program;
}

void Shader::FreeShader(GLuint shader)
{
	glDeleteShader(shader);
}

void Shader::Bind()
{
	glUseProgram(m_Handle);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

GLint Shader::GetUniformLocation(const std::string &name)
{
	auto iter = m_UniformCache.find(name);
	if (iter != m_UniformCache.end()) return iter->second;

	GLint location = glGetUniformLocation(m_Handle, name.c_str());
	if (location == -1)
	{
		spdlog::error("Trying to get location of uniform that doesn't exist: {}", name);
		return -1;
	}

	m_UniformCache[name] = location;
	return location;
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &mat)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetInt(const std::string &name, int val)
{
	glUniform1i(GetUniformLocation(name), val);
}

void Shader::SetUnsignedInt(const std::string &name, unsigned int val)
{
	glUniform1ui(GetUniformLocation(name), val);
}