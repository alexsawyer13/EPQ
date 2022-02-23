#include <Game/WorldGeneration.h>
#include <Core/Core.h>

#include <FastNoiseLite/FastNoiseLite.h>

#include <math.h>

int ChunkHeight = 80;

uint16_t grass;
uint16_t dirt;
uint16_t bedrock;
uint16_t stone;
uint16_t furnace_on;

FastNoiseLite noise;

void WorldGenerationSetup(unsigned int seed)
{
    noise.SetSeed(69696969);

	grass = BLOCK_PACK(core.BlockIds["grass"], 0);
	dirt = BLOCK_PACK(core.BlockIds["dirt"], 0);
	bedrock = BLOCK_PACK(core.BlockIds["bedrock"], 0);
	stone = BLOCK_PACK(core.BlockIds["stone"], 0);
	furnace_on = BLOCK_PACK(core.BlockIds["furnace_on"], 0);
}

//get the block type at a specific coordinate
uint16_t GenerateVoxel(int x, int y, int z)
{
    if (y == 0)
        return bedrock;

        //print(noise.GetSimplex(x, z));
    float simplex1 = noise.GetNoise(x * .8f, z * .8f) * 10;
    float simplex2 = noise.GetNoise(x * 3.0f, z * 3.0f) * 10 * (noise.GetNoise(x * .3f, z * .3f) + .5f);

    float heightMap = simplex1 + simplex2;

    //add the 2d noise to the middle of the terrain chunk
    float baseLandHeight = ChunkHeight * .5f + heightMap;

    //3d noise for caves and overhangs and such
    float caveNoise1 = noise.GetNoise(x * 5.0f, y * 10.0f, z * 5.0f);
    float caveMask = noise.GetNoise(x * .3f, z * .3f) + .3f;

    //stone layer heightmap
    float simplexStone1 = noise.GetNoise(x * 1.0f, z * 1.0f) * 10;
    float simplexStone2 = (noise.GetNoise(x * 5.0f, z * 5.0f) + .5f) * 20 * (noise.GetNoise(x * .3f, z * .3f) + .5f);

    float stoneHeightMap = simplexStone1 + simplexStone2;
    float baseStoneHeight = ChunkHeight * .25f + stoneHeightMap;


    //float cliffThing = noise.GetSimplex(x * 1f, z * 1f, y) * 10;
    //float cliffThingMask = noise.GetSimplex(x * .4f, z * .4f) + .3f;



    //BlockType blockType = BlockType.Air;
    uint16_t blockType = 0;

    //under the surface, dirt block
    if (y <= baseLandHeight)
    {
        blockType = dirt;

        //just on the surface, use a grass type
        if (y > baseLandHeight - 1)
            blockType = grass;

        if (y <= baseStoneHeight)
            blockType = stone;
    }


    if (caveNoise1 > fmaxf(caveMask, .2f))
        blockType = 0;

    /*if(blockType != BlockType.Air)
        blockType = BlockType.Stone;*/

        //if(blockType == BlockType.Air && noise.GetSimplex(x * 4f, y * 4f, z*4f) < 0)
          //  blockType = BlockType.Dirt;

        //if(Mathf.PerlinNoise(x * .1f, z * .1f) * 10 + y < TerrainChunk.chunkHeight * .5f)
        //    return BlockType.Grass;

    return blockType;
}

//uint16_t GenerateVoxel(int x, int y, int z)
//{
//	double smoothNoise = perlin.noise2D((double)x / 25, (double)z / 25) * 2 - 1;
//	int smoothHeight = (int)72 + smoothNoise * 4;
//
//	double mountainNoise = perlin.noise2D((double)x / 50, (double)z / 50) * 2 - 1;
//	int mountainHeight = (int)72 + mountainNoise * 90;
//
//
//	if (y == 0)
//		return bedrock;
//
//	if (smoothHeight > mountainHeight)
//	{
//		if (y > smoothHeight)
//			return 0;
//		else if (y == smoothHeight)
//			return grass;
//		else if (y > smoothHeight - 5)
//			return dirt;
//		else
//			return stone;
//	}
//	else
//	{
//		if (y > mountainHeight)
//			return 0;
//		else if (y > mountainHeight - 5 && mountainHeight - smoothHeight > 10) // Big mountains
//			return stone;
//		else if (y == mountainHeight)
//			return grass;
//		else if (y > smoothHeight - 5)
//			return dirt;
//		else
//			return stone;
//	}
//
//
//	return 0;
//}