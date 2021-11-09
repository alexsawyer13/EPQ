#pragma once

#include <Core/Assets.h>

#include <glad/glad.h>

struct TexParams
{
	GLint MinFilter;
	GLint MagFilter;
	GLint WrapS;
	GLint WrapT;
};

extern TexParams DEFAULT_TEX_PARAMS;

struct Texture2D
{
	GLuint Handle;
};

struct TexArray
{
	GLuint Handle;
	int Width, Height, Channels;
	int MaxTextures, NumTextures;
};

void Texture2DCreate(Texture2D *texture);
void Texture2DDestroy(Texture2D *texture);
void Texture2DBind(Texture2D *texture, unsigned int texUnit);
void Texture2DUnbind(unsigned int texUnit);
void Texture2DSetData(Texture2D *texture, Image* image);

void TexArrayCreate(TexArray *tex_array, int width, int height, int max_textures, TexParams params = DEFAULT_TEX_PARAMS);
void TexArrayDestroy(TexArray *tex_array);
int TexArrayAddImage(TexArray *tex_array, Image *image);
void TexArraySetParams(TexArray *tex_array, TexParams params);
void TexArrayBind(TexArray *tex_array, unsigned int tex_unit);