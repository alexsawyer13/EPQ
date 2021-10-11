#include <Graphics/TextureArray.h>
#include <Core/Assets.h>

#include <spdlog/spdlog.h>

TextureArray::TextureArray()
{

}

TextureArray::~TextureArray()
{
	Destroy();
}

void TextureArray::Create(int width, int height, int numTextures)
{
	Destroy();

	m_Width = width;
	m_Height = height;
	m_Channels = 4;
	m_NumTextures = 0;
	m_MaxTextures = numTextures;

	glGenTextures(1, &m_Handle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_Handle);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, numTextures);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void TextureArray::Destroy()
{
	if (m_Handle)
	{
		glDeleteTextures(1, &m_Handle);
	}
}

bool TextureArray::AddTexture(const std::string &path)
{
	if (m_NumTextures == m_MaxTextures)
	{
		spdlog::error("Cannot add texture {} to texture array, it is full", path);
		return false;
	}

	Stb_Image image(path, true);

	if (!image.Data)
	{
		spdlog::error("Stb failed to load image {}", path);
		return false;
	}

	if (image.Width > m_Width || image.Height > m_Height || image.Channels != m_Channels)
	{
		spdlog::error("Texture {} is incompatible with texture array", path);
		return false;
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_Handle);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_NumTextures, image.Width, image.Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, image.Data);

	m_NumTextures++;

	return true;
}

bool TextureArray::AddTexture(const Stb_Image &image)
{
	if (m_NumTextures == m_MaxTextures)
	{
		spdlog::error("Cannot add texture {} to texture array, it is full", image.Path);
		return false;
	}

	if (image.Width > m_Width || image.Height > m_Height || image.Channels != m_Channels)
	{
		spdlog::error("Texture {} is incompatible with texture array", image.Path);
		return false;
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_Handle);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_NumTextures, image.Width, image.Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, image.Data);

	m_NumTextures++;

	return true;
}

bool TextureArray::AddTexture(const std::string &path, unsigned char* data, int width, int height, int channels)
{
	if (m_NumTextures == m_MaxTextures)
	{
		spdlog::error("Cannot add texture {} to texture array, it is full", path);
		return false;
	}

	if (width > m_Width || height > m_Height || channels != m_Channels)
	{
		spdlog::error("Texture {} is incompatible with texture array", path);
		return false;
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_Handle);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_NumTextures, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

	m_NumTextures++;

	return true;
}

void TextureArray::Bind(unsigned int texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_Handle);
}