#pragma once

#include <Graphics/Shader.h>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <string>

struct GraphicsData;

class Cubemap
{
public:
	Cubemap();
	~Cubemap();

	void Create(const std::string &top, const std::string &bottom, const std::string &left, const std::string &right, const std::string &front, const std::string &back);
	void Destroy();

	void Render(const glm::mat4 view, const glm::mat4 proj);

	void Bind(unsigned int texUnit);

private:
	GLuint m_Handle;
};