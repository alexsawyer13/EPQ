#pragma once

#include <Core/Assets.h>

#include <glad/glad.h>

struct Texture2D
{
	GLuint Handle;
};

void Texture2DCreate(Texture2D *texture);
void Texture2DDestroy(Texture2D *texture);

void Texture2DBind(Texture2D *texture, unsigned int texUnit);
void Texture2DUnbind(unsigned int texUnit);

void Texture2DSetData(Texture2D *texture, Stb_Image* image);