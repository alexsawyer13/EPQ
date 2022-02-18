#include <Game/WorldGeneration.h>
#include <Core/Core.h>

siv::PerlinNoise perlin;

void WorldGenerationSetup(unsigned int seed)
{
	perlin.reseed(seed);
}

uint16_t GenerateVoxel(int x, int y, int z)
{
	double smoothNoise = perlin.noise2D((double)x/25, (double)z/25)*2-1;
	int smoothHeight = (int) 72 + smoothNoise * 4;

	double mountainNoise = perlin.noise2D((double)x / 50, (double)z / 50) * 2 - 1;
	int mountainHeight = (int) 72 + mountainNoise * 90;


	if (y == 0)
		return BLOCK_PACK(core.BlockIds["bedrock"], 0);

	if (smoothHeight > mountainHeight)
	{
	if (y > smoothHeight)
		return 0;
	else if (y == smoothHeight)
		return BLOCK_PACK(core.BlockIds["grass"], 0);
	else if (y > smoothHeight - 5)
		return BLOCK_PACK(core.BlockIds["dirt"], 0);
	else
		return BLOCK_PACK(core.BlockIds["stone"], 0);
	}
	else
	{
		if (y > mountainHeight)
			return 0;
		else if (y > mountainHeight - 5 && mountainHeight - smoothHeight > 10) // Big mountains
			return BLOCK_PACK(core.BlockIds["stone"], 0);
		else if (y == mountainHeight)
			return BLOCK_PACK(core.BlockIds["grass"], 0);
		else if (y > smoothHeight - 5)
			return BLOCK_PACK(core.BlockIds["dirt"], 0);
		else
			return BLOCK_PACK(core.BlockIds["stone"], 0);
	}


	return 0;
}

/*
uint16_t grass = BLOCK_PACK(core.BlockIds["grass"], 0);
uint16_t dirt = BLOCK_PACK(core.BlockIds["dirt"], 0);
uint16_t bedrock = BLOCK_PACK(core.BlockIds["bedrock"], 0);
uint16_t stone = BLOCK_PACK(core.BlockIds["stone"], 0);
uint16_t furnace_on = BLOCK_PACK(core.BlockIds["furnace_on"], 0);
*/