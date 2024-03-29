#include <Graphics/Texture.h>
#include <Core/Assets.h>
#include <Core/Core.h>

#include <spdlog/spdlog.h>

#include <vector>

TexParams DEFAULT_TEX_PARAMS = {
	GL_NEAREST,
	GL_NEAREST,
	GL_REPEAT,
	GL_REPEAT
};

void Texture2DCreate(Texture2D *texture)
{
	glGenTextures(1, &texture->_Handle);
}

void Texture2DDestroy(Texture2D *texture)
{
	glDeleteTextures(1, &texture->_Handle);
}

void Texture2DBind(Texture2D *texture, unsigned int texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, texture->_Handle);
}

void Texture2DUnbind(unsigned int texUnit)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2DSetData(Texture2D *texture, Image *image)
{
	Texture2DBind(texture, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->Width, image->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->Data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void TexArrayCreate(TexArray *tex_array, int width, int height, int max_textures, TexParams params)
{
	tex_array->Width = width;
	tex_array->Height = height;
	tex_array->Channels = 4;
	tex_array->NumTextures = 0;
	tex_array->MaxTextures = max_textures;

	glGenTextures(1, &tex_array->_Handle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex_array->_Handle);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, tex_array->MaxTextures);

	TexArraySetParams(tex_array, params);
}

void TexArrayDestroy(TexArray *tex_array)
{
	if (tex_array->_Handle)
		glDeleteTextures(1, &tex_array->_Handle);
}

// Returns index of texture in texarray, -1 if error
int TexArrayAddImage(TexArray *tex_array, Image *image)
{
	if (tex_array->NumTextures >= tex_array->MaxTextures)
	{
		spdlog::error("Cannot add texture to texture array, it is full");
		return -1;
	}

	if (image->Width > tex_array->Width || image->Height > tex_array->Height || image->Channels != tex_array->Channels)
	{
		spdlog::error("Texture is incompatible with texture array");
		return -1;
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, tex_array->_Handle);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, tex_array->NumTextures, image->Width, image->Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, image->Data);

	tex_array->NumTextures++;

	return tex_array->NumTextures - 1;
}

void TexArraySetParams(TexArray *tex_array, TexParams params)
{
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, params.MinFilter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, params.MagFilter);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, params.WrapS);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, params.WrapT);
}

void TexArrayBind(TexArray *tex_array, unsigned int tex_unit)
{
	glActiveTexture(GL_TEXTURE0 + tex_unit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex_array->_Handle);
}

void CubemapCreate(Cubemap *map, const std::string &top, const std::string &bottom, const std::string &left, const std::string &right, const std::string &front, const std::string &back)
{
	glGenTextures(1, &map->_Handle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, map->_Handle);

	std::string faces[6] = { right, left, top, bottom, front, back };

	for (unsigned int i = 0; i < 6; i++)
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

void CubemapDestroy(Cubemap *map)
{
	glDeleteTextures(1, &map->_Handle);
}

void CubemapRender(Cubemap *map, const glm::mat4 view, const glm::mat4 proj)
{
	glDepthMask(GL_FALSE);

	VaoBind(&core.vaos["cubemap"]);

	CubemapBind(map, 0);

	CoreShaderBind("cubemap");
	CoreShaderSetMat4("cubemap", "u_View", glm::mat4(glm::mat3(view)));
	CoreShaderSetMat4("cubemap", "u_Proj", proj);
	CoreShaderSetInt("cubemap", "u_Cubemap", 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

void CubemapBind(Cubemap *map, unsigned int texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, map->_Handle);
}