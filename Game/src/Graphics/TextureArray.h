#pragma once

#include <Core/Assets.h>

#include <glad/glad.h>

#include <string>

class TextureArray
{
public:
	TextureArray();
	~TextureArray();

	void Create(int width, int height, int numTextures);
	void Destroy();

	bool AddTexture(const Stb_Image &image);
	bool AddTexture(const std::string &p);
	bool AddTexture(const std::string &path, unsigned char *data, int width, int height, int channels);

	void Bind(unsigned int texUnit);

	int GetNumTextures() { return m_NumTextures; }

private:
	GLuint m_Handle;
	int m_Width, m_Height, m_Channels;
	int m_MaxTextures, m_NumTextures;
};