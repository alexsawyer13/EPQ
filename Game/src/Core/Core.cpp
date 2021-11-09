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

void LoadCoreData()
{
	core.block_texarray.Create(T_WIDTH, T_HEIGHT, 100);

	LoadGameData();

	// Load shaders
	core.shaders["optimisedtexarray"] = Shader();
	core.shaders["optimisedtexarray"].Create("optimisedtexarray.vert", "optimisedtexarray.frag");
	core.shaders["cubemap"] = Shader();
	core.shaders["cubemap"].Create("cubemap.vert", "cubemap.frag");
	core.shaders["texture"] = Shader();
	core.shaders["texture"].Create("texture.vert", "texture.frag");
	core.shaders["batch"] = Shader();
	core.shaders["batch"].Create("batch.vert", "batch.frag");

	// Load cubemap
	core.vaos["cubemap"] = {};
	VaoCreate(&core.vaos["cubemap"]);
	core.vbos["cubemap"] = {};
	VboCreate(&core.vbos["cubemap"]);

	VboData(&core.vbos["cubemap"], sizeof(CubeData::CubeVertices), CubeData::CubeVertices, GL_STATIC_DRAW);
	VaoAddVbo(&core.vaos["cubemap"], &core.vbos["cubemap"], { {3, GL_FLOAT} });

	core.cubemap.Create("skybox/top.jpg", "skybox/bottom.jpg", "skybox/left.jpg", "skybox/right.jpg", "skybox/front.jpg", "skybox/back.jpg");

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
	core.textures["crosshair"] = {};
	Texture2DCreate(&core.textures["crosshair"]);
	Image image("textures/crosshair.png", true);
	Texture2DSetData(&core.textures["crosshair"], &image);

	core.textures["none"] = {};
	Texture2DCreate(&core.textures["none"]);
	Image none_texture_image("textures/none.png", true);
	Texture2DSetData(&core.textures["none"], &none_texture_image);

	core.textures["hotbar_icon"] = {};
	Texture2DCreate(&core.textures["hotbar_icon"]);
	Image image2("textures/hotbar_icon_piskel.png", true);
	Texture2DSetData(&core.textures["hotbar_icon"], &image2);

	// Load objects
	WorldCreate(&core.world);
	PlayerCreate(&core.player, 10.0f, 5.0f);
	BatchCreate(&core.uirenderer);
}

void FreeCoreData()
{
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