#pragma once

#include <Graphics/Texture.h>
#include <Graphics/Shader.h>
#include <Graphics/TextureArray.h>
#include <Graphics/BatchRenderer.h>
#include <Game/World.h>
#include <Game/Player.h>
#include <Data/Blocks.h>

#include <GLFW/glfw3.h>

#include <unordered_map>

constexpr int T_WIDTH = 16;
constexpr int T_HEIGHT = 16;

struct Core
{
	// Platform
	GLFWwindow *window;
	Input input;

	// Game
	World world;
	Player player;

	// Render state
	BatchRenderer uirenderer;

	// OpenGL resources
	std::unordered_map<std::string, Texture2D> textures;
	std::unordered_map<std::string, Shader> shaders;
	std::unordered_map<std::string, VertexArray> vaos;
	std::unordered_map<std::string, VertexBuffer> vbos;
	std::unordered_map<std::string, IndexBuffer> ibos;
	TextureArray block_texarray;
	TextureArray item_texarray;
	Cubemap cubemap;

	// Blocks
	std::vector<Block> Blocks;
	std::vector<Item> Items;
	std::unordered_map<unsigned int, BlockTexture> BlockTextures;
	std::unordered_map<unsigned int, BlockTexture> ItemTextures;
	std::unordered_map<std::string, unsigned int> BlockTextureIds;
	std::unordered_map<std::string, unsigned int> ItemTextureIds;
	std::unordered_map<std::string, unsigned int> BlockIds;
	std::unordered_map<std::string, unsigned int> ItemIds;
};

extern Core core;

void LoadCoreData();
void FreeCoreData();