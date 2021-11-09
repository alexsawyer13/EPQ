#include <Data/Blocks.h>
#include <Core/Assets.h>
#include <Core/Core.h>
#include <Graphics/Texture.h>

#include <spdlog/spdlog.h>
#include <rapidjson/document.h>

#include <unordered_map>
#include <vector>

void LoadGameData()
{
	// Load file and parse it into json
	std::string blockFile = ReadAsset("data/blocks.json");
	rapidjson::Document blockJson;
	blockJson.Parse(blockFile.c_str());

	unsigned int currentId = 0;

	// Loop through blocks
	for (auto iter = blockJson.MemberBegin(); iter != blockJson.MemberEnd(); ++iter)
	{
		Block block;
		block.Name = iter->name.GetString();
		block.Id = core.Blocks.size();

		block.IsBlock = iter->value["IsBlock"].GetBool();
		block.IsTransparent = iter->value["IsTransparent"].GetBool();
		block.HasMetadata = iter->value["HasMetadata"].GetBool();

		const char *collider = iter->value["Collider"].GetString();
		if (!strcmp(collider, "none"))
			block.Collider = Collider::None;
		else if (!strcmp(collider, "block"))
			block.Collider = Collider::Block;

		std::string texture = iter->value["Top"].GetString(); // Get texture from json
		auto texIter = core.BlockTextureIds.find(texture); // See if it's already been cached
		if (texIter == core.BlockTextureIds.end()) // If it hasn't been cached
		{
			BlockTexture blockTexture = LoadBlockTexture(texture, currentId); // Load the textures
			core.BlockTextureIds[texture] = currentId; // Add it to the hashmap of names to ids with correct id
			core.BlockTextures[currentId] = blockTexture; // Add it to the hasmap of ids to textures
			currentId += blockTexture.AnimationFrames;
		}
		block.Top = core.BlockTextureIds[texture]; // Set block's data to the correct id

		texture = iter->value["Bottom"].GetString(); // Get texture from json
		texIter = core.BlockTextureIds.find(texture); // See if it's already been cached
		if (texIter == core.BlockTextureIds.end()) // If it hasn't been cached
		{
			BlockTexture blockTexture = LoadBlockTexture(texture, currentId); // Load the textures
			core.BlockTextureIds[texture] = currentId; // Add it to the hashmap of names to ids with correct id
			core.BlockTextures[currentId] = blockTexture; // Add it to the hasmap of ids to textures
			currentId += blockTexture.AnimationFrames;
		}
		block.Bottom = core.BlockTextureIds[texture]; // Set block's data to the correct id

		texture = iter->value["Left"].GetString(); // Get texture from json
		texIter = core.BlockTextureIds.find(texture); // See if it's already been cached
		if (texIter == core.BlockTextureIds.end()) // If it hasn't been cached
		{
			BlockTexture blockTexture = LoadBlockTexture(texture, currentId); // Load the textures
			core.BlockTextureIds[texture] = currentId; // Add it to the hashmap of names to ids with correct id
			core.BlockTextures[currentId] = blockTexture; // Add it to the hasmap of ids to textures
			currentId += blockTexture.AnimationFrames;
		}
		block.Left = core.BlockTextureIds[texture]; // Set block's data to the correct id

		texture = iter->value["Right"].GetString(); // Get texture from json
		texIter = core.BlockTextureIds.find(texture); // See if it's already been cached
		if (texIter == core.BlockTextureIds.end()) // If it hasn't been cached
		{
			BlockTexture blockTexture = LoadBlockTexture(texture, currentId); // Load the textures
			core.BlockTextureIds[texture] = currentId; // Add it to the hashmap of names to ids with correct id
			core.BlockTextures[currentId] = blockTexture; // Add it to the hasmap of ids to textures
			currentId += blockTexture.AnimationFrames;
		}
		block.Right = core.BlockTextureIds[texture]; // Set block's data to the correct id

		texture = iter->value["Front"].GetString(); // Get texture from json
		texIter = core.BlockTextureIds.find(texture); // See if it's already been cached
		if (texIter == core.BlockTextureIds.end()) // If it hasn't been cached
		{
			BlockTexture blockTexture = LoadBlockTexture(texture, currentId); // Load the textures
			core.BlockTextureIds[texture] = currentId; // Add it to the hashmap of names to ids with correct id
			core.BlockTextures[currentId] = blockTexture; // Add it to the hasmap of ids to textures
			currentId += blockTexture.AnimationFrames;
		}
		block.Front = core.BlockTextureIds[texture]; // Set block's data to the correct id

		texture = iter->value["Back"].GetString(); // Get texture from json
		texIter = core.BlockTextureIds.find(texture); // See if it's already been cached
		if (texIter == core.BlockTextureIds.end()) // If it hasn't been cached
		{
			BlockTexture blockTexture = LoadBlockTexture(texture, currentId); // Load the textures
			core.BlockTextureIds[texture] = currentId; // Add it to the hashmap of names to ids with correct id
			core.BlockTextures[currentId] = blockTexture; // Add it to the hasmap of ids to textures
			currentId += blockTexture.AnimationFrames;
		}
		block.Back = core.BlockTextureIds[texture]; // Set block's data to the correct id

		core.BlockIds[block.Name] = core.Blocks.size();
		core.Blocks.push_back(block);
	}

	// Load items
	//std::string itemFile = ReadAsset("data/items.json");
	//rapidjson::Document itemJson;
	//itemJson.Parse(itemFile.c_str());

	// Loop through items
	//for (auto iter = itemJson.MemberBegin(); iter != itemJson.MemberEnd(); ++iter)
	//{
	//	Item item;
	//	item.Name = iter->name.GetString();
	//	item.Id = core.Items.size();

	//	std::string texture = iter->value["Icon"].GetString(); // Get texture from json
	//	auto texIter = core.ItemTextureIds.find(texture); // See if it's already been cached
	//	if (texIter == core.ItemTextureIds.end()) // If it hasn't been cached
	//	{
	//		BlockTexture itemTexture = LoadTexture(texture, currentId); // Load the textures
	//		core.ItemTextureIds[texture] = currentId; // Add it to the hashmap of names to ids with correct id
	//		core.ItemTextures[currentId] = itemTexture; // Add it to the hasmap of ids to textures
	//		currentId += itemTexture.AnimationFrames;
	//	}
	//	item.Icon = core.ItemTextureIds[texture]; // Set block's data to the correct id

	//	core.ItemIds[item.Name] = item.Id;
	//	core.Items.push_back(item);
	//}
}

BlockTexture LoadBlockTexture(const std::string &path, unsigned int currentId)
{
	BlockTexture texture;
	texture.Id = currentId;
	texture.Name = path;

	Image image("textures/" + path + ".png", true);

	if (image.Width != T_WIDTH)
	{
		spdlog::error("Can't load texture {} since width isn't {}", path, T_WIDTH);
		texture.AnimationFrames = 0;
	}

	if (image.Height == T_HEIGHT)
	{
		TexArrayAddImage(&core.block_texarray, &image);
		texture.AnimationFrames = 1;
	}

	else
	{
		int num_images = image.Height / 16;
		if (num_images * 16 != image.Height)
		{
			spdlog::error("Can't load texture {} since height isn't a multiple of {}", path, T_HEIGHT);
			texture.AnimationFrames = 0;
		}

		for (int i = 0; i < num_images; i++)
		{
			unsigned char *current_image_data = image.Data + T_WIDTH * T_HEIGHT * image.Channels * i;
			Image current_image(current_image_data, T_WIDTH, T_HEIGHT, image.Channels, ImageOwnership::BORROW);
			TexArrayAddImage(&core.block_texarray, &current_image);
		}

		texture.AnimationFrames = num_images;
	}

	return texture;
}