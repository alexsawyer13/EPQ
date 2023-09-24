#include <Graphics/Cubemap.h>
#include <Core/Assets.h>
#include <Core/Core.h>

#include <vector>

Cubemap::Cubemap()
{

}

Cubemap::~Cubemap()
{
	Destroy();
}

void Cubemap::Create(const std::string &top, const std::string &bottom, const std::string &left, const std::string &right, const std::string &front, const std::string &back)
{
	Destroy();

	glGenTextures(1, &m_Handle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle);

	std::vector<std::string> faces = { right, left, top, bottom, front, back };

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		Image image(faces[i], false);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image.Width, image.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::Destroy()
{
	if (m_Handle)
	{
		glDeleteTextures(1, &m_Handle);
	}
}

void Cubemap::Render(const glm::mat4 view, const glm::mat4 proj)
{
	glDepthMask(GL_FALSE);

	VaoBind(&core.vaos["cubemap"]);

	Bind(0);

	CoreShaderBind("cubemap");
	CoreShaderSetMat4("cubemap", "u_View", glm::mat4(glm::mat3(view)));
	CoreShaderSetMat4("cubemap", "u_Proj", proj);
	CoreShaderSetInt("cubemap", "u_Cubemap", 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

void Cubemap::Bind(unsigned int texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle);
}