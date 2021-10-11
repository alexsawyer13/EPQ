#include <Game/World.h>
#include <Core/Core.h>
#include <Graphics/Shader.h>
#include <Maths/Maths.h>

#include <spdlog/spdlog.h>

void WorldCreate(World *world)
{
	for (int x = -RENDERDISTANCE; x <= RENDERDISTANCE; x++)
	{
		for (int z = -RENDERDISTANCE; z <= RENDERDISTANCE; z++)
		{
			Chunk chunk{ 0 };
			ChunkCreate(&chunk, x, z);
			world->Chunks.push_back(chunk);
			world->ActiveChunks[CHUNK_HASH(x, z)] = world->Chunks.size() - 1;
		}
	}

	for (int i = 0; i < world->Chunks.size(); i++)
	{
		Chunk &chunk = world->Chunks[i];
		Chunk *posx = WorldGetChunk(world, chunk.X + 1, chunk.Z);
		Chunk *negx = WorldGetChunk(world, chunk.X - 1, chunk.Z);
		Chunk *posz = WorldGetChunk(world, chunk.X, chunk.Z + 1);
		Chunk *negz = WorldGetChunk(world, chunk.X, chunk.Z - 1);
		ChunkSetNeighbours(&chunk, posx, negx, posz, negz);
		world->PendingMesh.push_back(i);
	}
}

void WorldDestroy(World *world)
{

}

void WorldDrawChunks(World *world)
{
	for (auto iter = world->ActiveChunks.begin(); iter != world->ActiveChunks.end(); ++iter)
	{
		Chunk &chunk = world->Chunks[iter->second];
		if (chunk.Visible)
		{
			core.shaders["optimisedtexarray"].SetMat4("u_Model", chunk.Model);
			VaoDraw(&world->Chunks[iter->second].Vao);
		}
	}
}

Chunk *WorldGetChunk(World *world, int x, int z)
{
	ChunkCoord cc = { x, z };
	uint64_t hash = cc.Hash();

	world->ActiveChunks.find(hash);
	auto active = world->ActiveChunks.find(hash);
	if (active != world->ActiveChunks.end())
		return &world->Chunks[world->ActiveChunks[hash]];

	auto inactive = world->InactiveChunks.find(hash);
	if (inactive != world->InactiveChunks.end())
		return &world->Chunks[world->InactiveChunks[hash]];

	return nullptr;
}

Block &WorldGetBlock(World *world, int x, int y, int z)
{
	if (y > CHUNK_HEIGHT - 1 || y < 0)
	{
		spdlog::error("Trying to access block that is above or below world, returning block id 0");
		return core.Blocks[0];
	}

	int chunkX = RoundToLowest((float)x / CHUNK_WIDTH);
	int chunkZ = RoundToLowest((float)z / CHUNK_WIDTH);

	int localX = x - CHUNK_WIDTH * chunkX;
	int localZ = z - CHUNK_WIDTH * chunkZ;

	auto iter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ));
	if (iter == world->ActiveChunks.end()) // Block isn't in active chunk
	{
		spdlog::error("Trying to access block that is outside of active chunks, returning block id 0");
		return core.Blocks[0];
	}

	Chunk &chunk = world->Chunks[iter->second];
	return core.Blocks[BLOCK_ID(chunk.Data[CHUNK_INDEX_OF(localX, y, localZ)])];
}

void WorldSetBlock(World *world, int x, int y, int z, uint16_t block_id)
{
	if (y > CHUNK_HEIGHT - 1 || y < 0)
	{
		spdlog::error("Trying to set block that is above or below world");
		return;
	}

	int chunkX = RoundToLowest((float)x / CHUNK_WIDTH);
	int chunkZ = RoundToLowest((float)z / CHUNK_WIDTH);

	int localX = x - CHUNK_WIDTH * chunkX;
	int localZ = z - CHUNK_WIDTH * chunkZ;

	auto iter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ));
	if (iter == world->ActiveChunks.end()) // Block isn't in active chunk
	{
		spdlog::error("Trying to set block that is outside of active chunks");
		return;
	}

	world->Chunks[iter->second].Data[CHUNK_INDEX_OF(localX, y, localZ)] = BLOCK_PACK(block_id, 0);

	WorldPushChunkMeshUpdate(world, iter->second);

	if (localX == CHUNK_WIDTH - 1)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX + 1, chunkZ));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshUpdate(world, niter->second);
		}
	}
	if (localX == 0)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX - 1, chunkZ));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshUpdate(world, niter->second);
		}
	}
	if (localZ == CHUNK_WIDTH - 1)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ + 1));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshUpdate(world, niter->second);
		}
	}
	if (localZ == 0)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ - 1));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshUpdate(world, niter->second);
		}
	}
}

void WorldBreakBlock(World *world, int x, int y, int z)
{
	if (y > CHUNK_HEIGHT - 1 || y < 0)
	{
		spdlog::error("Trying to break block that is above or below world");
		return;
	}

	int chunkX = RoundToLowest((float)x / CHUNK_WIDTH);
	int chunkZ = RoundToLowest((float)z / CHUNK_WIDTH);

	int localX = x - CHUNK_WIDTH * chunkX;
	int localZ = z - CHUNK_WIDTH * chunkZ;

	auto iter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ));
	if (iter == world->ActiveChunks.end()) // Block isn't in active chunk
	{
		spdlog::error("Trying to break block that is outside of active chunks");
		return;
	}

	world->Chunks[iter->second].Data[CHUNK_INDEX_OF(localX, y, localZ)] = 0;

	if (localX == CHUNK_WIDTH - 1)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX + 1, chunkZ));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshUpdate(world, niter->second);
		}
	}
	if (localX == 0)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX - 1, chunkZ));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshUpdate(world, niter->second);
		}
	}
	if (localZ == CHUNK_WIDTH - 1)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ + 1));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshUpdate(world, niter->second);
		}
	}
	if (localZ == 0)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ - 1));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshUpdate(world, niter->second);
		}
	}

	WorldPushChunkMeshUpdate(world, iter->second);
}

void WorldUpdate(World *world)
{
	if (world->PendingMesh.size() > 0)
	{
		Chunk &chunk = world->Chunks[world->PendingMesh.front()];
		world->PendingMesh.pop_front();

		ChunkBuildMesh(&chunk);
	}
}

void WorldPushChunkMeshUpdate(World *world, int chunk_id)
{
	bool alreadyPending = false;
	for (int i = 0; i < world->PendingMesh.size(); i++)
	{
		if (world->PendingMesh[i] == chunk_id)
		{
			alreadyPending = true;
		}
	}

	if (!alreadyPending)
	{
		world->PendingMesh.push_back(chunk_id);
	}
}