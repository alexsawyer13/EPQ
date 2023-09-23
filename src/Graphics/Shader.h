#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

class Shader
{
public:
	Shader();
	~Shader();

	void Create(const std::string &vertexPath, const std::string &fragmentPath);
	void Destroy();

	void Bind();
	static void Unbind();

	void SetMat4(const std::string &name, const glm::mat4 &mat);
	void SetInt(const std::string &name, int val);
	void SetUnsignedInt(const std::string &name, unsigned int val);
	void SetFloat(const std::string &name, float val);
	void SetFloat4(const std::string &name, const glm::vec4 &val);

private:
	GLuint LoadShader(const std::string &source, GLenum type);
	GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
	void FreeShader(GLuint shader);

	GLint GetUniformLocation(const std::string &name);

	GLuint m_Handle;
	std::unordered_map<std::string, GLint> m_UniformCache;
};