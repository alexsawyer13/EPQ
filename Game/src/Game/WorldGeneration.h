#pragma once

#include <Game/Chunk.h>

#include <PerlinNoise/PerlinNoise.hpp>

void WorldGenerationSetup(unsigned int seed);
uint16_t GenerateVoxel(int x, int y, int z);