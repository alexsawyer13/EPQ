#include <Game/WorldGeneration.h>
#include <Core/Core.h>

#include <FastNoiseLite/FastNoiseLite.h>

#include <math.h>
#include <array>

int ChunkHeight = 80;

uint16_t grass;
uint16_t dirt;
uint16_t bedrock;
uint16_t stone;
uint16_t furnace_on;

FastNoiseLite noise;

void WorldGenerationSetup(unsigned int seed)
{
    noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Perlin);
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
    if (y == 0) return bedrock;

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

    return blockType;
}

std::array<uint16_t, CHUNK_HEIGHT> GenerateVoxelStrip(int x, int z)
{
    std::array<uint16_t, CHUNK_HEIGHT> strip;
    strip[0] = bedrock;

    float simplex1 = noise.GetNoise(x * .8f, z * .8f) * 10;
    float simplex2 = noise.GetNoise(x * 3.0f, z * 3.0f) * 10 * (noise.GetNoise(x * .3f, z * .3f) + .5f);

    float heightMap = simplex1 + simplex2;

    //add the 2d noise to the middle of the terrain chunk
    float baseLandHeight = ChunkHeight * .5f + heightMap;

    //3d noise for caves and overhangs and such
    float caveMask = noise.GetNoise(x * .3f, z * .3f) + .3f;
    
    //stone layer heightmap
    float simplexStone1 = noise.GetNoise(x * 1.0f, z * 1.0f) * 10;
    float simplexStone2 = (noise.GetNoise(x * 5.0f, z * 5.0f) + .5f) * 20 * (noise.GetNoise(x * .3f, z * .3f) + .5f);
    float stoneHeightMap = simplexStone1 + simplexStone2;
    float baseStoneHeight = ChunkHeight * .25f + stoneHeightMap;

    for (int y = 1; y < CHUNK_HEIGHT; y++)
    {
        //3d noise for caves and overhangs and such
        float caveNoise1 = noise.GetNoise(x * 5.0f, y * 10.0f, z * 5.0f);

        strip[y] = 0;
        //under the surface, dirt block
        if (y <= baseLandHeight)
        {
            strip[y] = dirt;

            //just on the surface, use a grass type
            if (y > baseLandHeight - 1)
                strip[y] = grass;

            if (y <= baseStoneHeight)
                strip[y] = stone;
        }


        if (caveNoise1 > fmaxf(caveMask, .2f))
            strip[y] = 0;
    }

    return strip;
}