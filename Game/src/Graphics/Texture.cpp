#include <Graphics/Texture.h>

#include <spdlog/spdlog.h>

void Texture2DCreate(Texture2D *texture)
{
	glGenTextures(1, &texture->Handle);
}

void Texture2DDestroy(Texture2D *texture)
{
	glDeleteTextures(1, &texture->Handle);
}

void Texture2DBind(Texture2D *texture, unsigned int texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, texture->Handle);
}

void Texture2DUnbind(unsigned int texUnit)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2DSetData(Texture2D *texture, Stb_Image *image)
{
	Texture2DBind(texture, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->Width, image->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->Data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}