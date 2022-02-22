#include <Game/WorldGeneration.h>
#include <Core/Core.h>

siv::PerlinNoise perlin;

uint16_t grass;
uint16_t dirt;
uint16_t bedrock;
uint16_t stone;
uint16_t furnace_on;

void WorldGenerationSetup(unsigned int seed)
{
	perlin.reseed(seed);

	grass = BLOCK_PACK(core.BlockIds["grass"], 0);
	dirt = BLOCK_PACK(core.BlockIds["dirt"], 0);
	bedrock = BLOCK_PACK(core.BlockIds["bedrock"], 0);
	stone = BLOCK_PACK(core.BlockIds["stone"], 0);
	furnace_on = BLOCK_PACK(core.BlockIds["furnace_on"], 0);
}

uint16_t GenerateVoxel(int x, int y, int z)
{
	double smoothNoise = perlin.noise2D((double)x/25, (double)z/25)*2-1;
	int smoothHeight = (int) 72 + smoothNoise * 4;

	double mountainNoise = perlin.noise2D((double)x / 50, (double)z / 50) * 2 - 1;
	int mountainHeight = (int) 72 + mountainNoise * 90;


	if (y == 0)
		return bedrock;

	if (smoothHeight > mountainHeight)
	{
		if (y > smoothHeight)
			return 0;
		else if (y == smoothHeight)
			return grass;
		else if (y > smoothHeight - 5)
			return dirt;
		else
			return stone;
	}
	else
	{
		if (y > mountainHeight)
			return 0;
		else if (y > mountainHeight - 5 && mountainHeight - smoothHeight > 10) // Big mountains
			return stone;
		else if (y == mountainHeight)
			return grass;
		else if (y > smoothHeight - 5)
			return dirt;
		else
			return stone;
	}


	return 0;
}