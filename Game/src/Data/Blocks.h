#pragma once

#include <Graphics/TextureArray.h>
#include <Graphics/Shader.h>
#include <Graphics/Buffers.h>
#include <Graphics/Cubemap.h>
#include <Graphics/Texture.h>
#include <Core/Input.h>
#include <Physics/AABB.h>

#include <rapidjson/document.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <unordered_map>

struct Block
{
	std::string Name;
	unsigned int Id;
	
	bool IsBlock;
	bool IsTransparent;
	bool HasMetadata;

	Collider Collider;

	unsigned int ItemDrop;

	union
	{
		unsigned int Faces[6];
		struct 
		{
			unsigned int Top;
			unsigned int Bottom;
			unsigned int Left;
			unsigned int Right;
			unsigned int Front;
			unsigned int Back;
		};
	};
};

struct Item
{
	std::string Name;
	unsigned int Id;

	unsigned int StackSize;

	unsigned int Icon;
};

struct BlockTexture
{
	std::string Name;
	unsigned int Id;

	unsigned int AnimationFrames;
};

void LoadGameData();
BlockTexture LoadTexture(const std::string &path, unsigned int currentId);