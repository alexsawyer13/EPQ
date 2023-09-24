#include <Graphics/Shader.h>
#include <Core/Assets.h>

#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>

GLuint _LoadShader(const std::string &source, GLenum type);
GLuint _LinkProgram(GLuint vertexShader, GLuint fragmentShader);
GLint _GetUniformLocation(const std::string &name);

int ShaderCreate(Shader *shader, const std::string &vertexPath, const std::string &fragmentPath)
{
	std::string vertexSource = ReadAsset("shaders/" + vertexPath);
	std::string fragmentSource = ReadAsset("shaders/" + fragmentPath);

	GLuint vertexShader = _LoadShader(vertexSource, GL_VERTEX_SHADER);
	if (vertexShader == 0) return false;

	GLuint fragmentShader = _LoadShader(fragmentSource, GL_FRAGMENT_SHADER);
	if (fragmentShader == 0) return false;

	shader->_Handle = _LinkProgram(vertexShader, fragmentShader);
	if (shader->_Handle == 0) return false;

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

void ShaderDestroy(Shader *shader)
{
	glDeleteProgram(shader->_Handle);
	shader->_UniformCache.clear();
	shader->_Handle = 0;
}

GLuint _LoadShader(const std::string &source, GLenum type)
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
		spdlog::critical("Failed to compile shader: \n{}\n{}", source, log);
		delete[] log;

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint _LinkProgram(GLuint vertexShader, GLuint fragmentShader)
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

		glDeleteProgram(program);
		return 0;
	}

	return program;
}

void ShaderBind(Shader *shader)
{
	glUseProgram(shader->_Handle);
}

void ShaderUnbind()
{
	glUseProgram(0);
}

GLint _GetUniformLocation(Shader *shader, const std::string &name)
{
	auto iter = shader->_UniformCache.find(name);
	if (iter != shader->_UniformCache.end()) return iter->second;

	GLint location = glGetUniformLocation(shader->_Handle, name.c_str());
	if (location == -1)
	{
		spdlog::error("Trying to get location of uniform that doesn't exist: {}", name);
		return -1;
	}

	shader->_UniformCache[name] = location;
	return location;
}

void ShaderSetMat4(Shader *shader, const std::string &name, const glm::mat4 &mat)
{
	glUniformMatrix4fv(_GetUniformLocation(shader, name), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderSetInt(Shader *shader, const std::string &name, int val)
{
	glUniform1i(_GetUniformLocation(shader, name), val);
}

void ShaderSetUnsignedInt(Shader *shader, const std::string &name, unsigned int val)
{
	glUniform1ui(_GetUniformLocation(shader, name), val);
}

void ShaderSetFloat(Shader *shader, const std::string &name, float val)
{
	glUniform1f(_GetUniformLocation(shader, name), val);
}

void ShaderSetFloat4(Shader *shader, const std::string &name, const glm::vec4 &val)
{
	glUniform4f(_GetUniformLocation(shader, name), val[0], val[1], val[2], val[3]);
}