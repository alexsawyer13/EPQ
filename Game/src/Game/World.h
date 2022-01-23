#pragma once

#include <Game/Chunk.h>

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <deque>
#include <thread>
#include <optional>

constexpr int RENDERDISTANCE = 8;
constexpr int ChunkMeshesPerFrame = 1;

#define CHUNK_HASH(x, z) ((((uint64_t)(x)) << 32) + ((uint64_t)(z)))

struct World
{
	std::vector<Chunk> Chunks; // TODO: CAN'T USE POINTERS TO CHUNKS BECAUSE CHUNKS ARE IN A VECTOR WHOOPS

	std::unordered_map<uint64_t, int> ActiveChunks;
	std::unordered_map<uint64_t, int> InactiveChunks;

	std::deque<int> PendingMesh;
};

void WorldCreate(World *world);
void WorldDestroy(World *world);

void WorldDrawChunks(World *world);
void WorldPushChunkMeshUpdate(World *world, int chunk_id);

Chunk *WorldGetChunk(World *world, int x, int z);
Chunk *WorldGetChunkIfActive(World *world, int x, int z);
Block& WorldGetBlock(World *world, int x, int y, int z);
void WorldSetBlock(World *world, int x, int y, int z, uint16_t block_id);
void WorldBreakBlock(World *world, int x, int y, int z);

void WorldUpdate(World *world);

struct ChunkCoord
{
	int x, z;

	uint64_t Hash()
	{
		return CHUNK_HASH(x, z);
	}

	bool operator==(ChunkCoord &rhs) const
	{
		return (x == rhs.x && z == rhs.z);
	}
};