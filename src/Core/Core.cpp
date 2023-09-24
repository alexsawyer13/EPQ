#include <Core/Core.h>
#include <Data/CubeData.h>

Core core;

static float rectangleVertices[] = {
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Top right
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Top left
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom right
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Bottom left
};

unsigned int rectangleIndices[] = {
	0, 1, 2,
	2, 1, 3,
};

void _LoadTexture(const std::string &name, const std::string &src);
void _LoadShader(const std::string &name, const std::string &vert, const std::string &frag);

void LoadCoreData()
{
	TexArrayCreate(&core.block_texarray, T_WIDTH, T_HEIGHT, 100);

	LoadGameData();

	// Load shaders
	_LoadShader("optimisedtexarray", "optimisedtexarray.vert", "optimisedtexarray.frag");
	_LoadShader("cubemap", "cubemap.vert", "cubemap.frag");
	_LoadShader("texture", "texture.vert", "cubemap.frag");
	_LoadShader("batch", "batch.vert", "batch.frag");
	_LoadShader("highlight", "highlight_shader.vert", "highlight_shader.frag");

	// Load cubemap
	core.vaos["cubemap"] = {};
	VaoCreate(&core.vaos["cubemap"]);
	core.vbos["cubemap"] = {};
	VboCreate(&core.vbos["cubemap"]);

	VboData(&core.vbos["cubemap"], sizeof(CubeData::CubeVertices), CubeData::CubeVertices, GL_STATIC_DRAW);
	VaoAddVbo(&core.vaos["cubemap"], &core.vbos["cubemap"], { {3, GL_FLOAT} });

	//core.cubemap.Create("oldskybox/top.jpg", "oldskybox/bottom.jpg", "oldskybox/left.jpg", "oldskybox/right.jpg", "oldskybox/front.jpg", "oldskybox/back.jpg");
	CubemapCreate(&core.cubemap, "skybox/top.png", "skybox/bottom.png", "skybox/side.png", "skybox/side.png", "skybox/side.png", "skybox/side.png");

	// Load textured rectangle

	core.vaos["rectangle"] = {};
	VaoCreate(&core.vaos["rectangle"]);
	core.vbos["rectangle"] = {};
	VboCreate(&core.vbos["rectangle"]);
	core.ibos["rectangle"] = {};
	IboCreate(&core.ibos["rectangle"]);

	VboData(&core.vbos["rectangle"], sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
	VaoAddVbo(&core.vaos["rectangle"], &core.vbos["rectangle"], { {3, GL_FLOAT}, {2, GL_FLOAT} });

	IboData(&core.ibos["rectangle"], sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW);
	VaoSetIbo(&core.vaos["rectangle"], &core.ibos["rectangle"]);

	// Load textures

	_LoadTexture("none", "textures/none.png");
	_LoadTexture("crosshair", "textures/crosshair.png");
	_LoadTexture("hotbar_icon", "textures/hotbar_icon_piskel.png");
	_LoadTexture("hotbar_icon_on", "textures/hotbar_icon_on_piskel.png");

	_LoadTexture("dirt", "textures/block/dirt.png");
	_LoadTexture("grass", "textures/block/grass_block_side.png");
	_LoadTexture("stone", "textures/block/stone.png"); 
	_LoadTexture("cobblestone", "textures/block/cobblestone.png");
	_LoadTexture("oak_planks", "textures/block/oak_planks.png");
	_LoadTexture("oak_log", "textures/block/oak_log.png");
	_LoadTexture("bedrock", "textures/block/bedrock.png");
	_LoadTexture("furnace_on", "textures/block/furnace_front.png");

	// Load rectangle for highlighting block faces
	core.vaos["highlight"] = {};
	VaoCreate(&core.vaos["highlight"]);
	core.vbos["highlight"] = {};
	VboCreate(&core.vbos["highlight"]);
	VboData(&core.vbos["highlight"], 12 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	core.ibos["highlight"] = {};
	IboCreate(&core.ibos["highlight"]);
	IboData(&core.ibos["highlight"], 6 * sizeof(unsigned int), CubeData::RectangleIndices, GL_STATIC_DRAW);
	VaoAddVbo(&core.vaos["highlight"], &core.vbos["highlight"], { {3, GL_FLOAT} });
	VaoSetIbo(&core.vaos["highlight"], &core.ibos["highlight"]);

	// Load objects
	WorldCreate(&core.world);
	PlayerCreate(&core.player, 10.0f, 5.0f);
	BatchCreate(&core.uirenderer);
}

void FreeCoreData()
{
	for (auto &shader : core.shaders)
	{
		ShaderDestroy(&shader.second);
	}

	for (auto &iter : core.textures)
	{
		Texture2DDestroy(&iter.second);
	}

	for (auto &iter : core.vaos)
	{
		VaoDestroy(&iter.second);
	}

	for (auto &iter : core.vbos)
	{
		VboDestroy(&iter.second);
	}

	for (auto &iter : core.ibos)
	{
		IboDestroy(&iter.second);
	}
}

void _LoadTexture(const std::string &name, const std::string &src)
{
	core.textures[name] = {};
	Texture2DCreate(&core.textures[name]);
	Image image(src, true);
	Texture2DSetData(&core.textures[name], &image);
}

void _LoadShader(const std::string &name, const std::string &vert, const std::string &frag)
{
	core.shaders[name] = {};
	ShaderCreate(&core.shaders[name], vert, frag);
}

void CoreShaderBind(const std::string &shader)
{
	ShaderBind(&core.shaders[shader]);
}

void CoreShaderSetMat4(const std::string &shader, const std::string &name, const glm::mat4 &mat)
{
	ShaderSetMat4(&core.shaders[shader], name, mat);
}

void CoreShaderSetInt(const std::string &shader, const std::string &name, int val)
{
	ShaderSetInt(&core.shaders[shader], name, val);
}

void CoreShaderSetUnsignedInt(const std::string &shader, const std::string &name, unsigned int val)
{
	ShaderSetUnsignedInt(&core.shaders[shader], name, val);
}

void CoreShaderSetFloat(const std::string &shader, const std::string &name, float val)
{
	ShaderSetFloat(&core.shaders[shader], name, val);
}

void CoreShaderSetFloat4(const std::string &shader, const std::string &name, const glm::vec4 &val)
{
	ShaderSetFloat4(&core.shaders[shader], name, val);
}