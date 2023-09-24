#pragma once

#include <Core/Profiler.h>
#include <Core/Input.h>
#include <Graphics/Texture.h>
#include <Graphics/Shader.h>
#include <Graphics/BatchRenderer.h>
#include <Graphics/Cubemap.h>
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
	Profiler profiler;

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
	TexArray block_texarray;
	TexArray item_texarray;
	Cubemap cubemap;

	// Data
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

void CoreShaderBind(const std::string &shader);
void CoreShaderSetMat4(const std::string &shader, const std::string &name, const glm::mat4 &mat);
void CoreShaderSetInt(const std::string &shader, const std::string &name, int val);
void CoreShaderSetUnsignedInt(const std::string &shader, const std::string &name, unsigned int val);
void CoreShaderSetFloat(const std::string &shader, const std::string &name, float val);
void CoreShaderSetFloat4(const std::string &shader, const std::string &name, const glm::vec4 &val);