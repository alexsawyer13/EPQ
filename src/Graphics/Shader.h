#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

struct Shader
{
	GLuint _Handle;
	std::unordered_map<std::string, GLint> _UniformCache;
};

int ShaderCreate(Shader *shader, const std::string &vert, const std::string &frag);
void ShaderDestroy(Shader *shader);

void ShaderBind(Shader *shader);

void ShaderSetMat4(Shader *shader, const std::string &name, const glm::mat4 &mat);
void ShaderSetInt(Shader *shader, const std::string &name, int val);
void ShaderSetUnsignedInt(Shader *shader, const std::string &name, unsigned int val);
void ShaderSetFloat(Shader *shader, const std::string &name, float val);
void ShaderSetFloat4(Shader *shader, const std::string &name, const glm::vec4 &val);