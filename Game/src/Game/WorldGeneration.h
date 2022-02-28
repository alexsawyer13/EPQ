#pragma once

#include <Game/Chunk.h>

#include <array>

void WorldGenerationSetup(unsigned int seed);
uint16_t GenerateVoxel(int x, int y, int z);
std::array<uint16_t, CHUNK_HEIGHT> GenerateVoxelStrip(int x, int z);