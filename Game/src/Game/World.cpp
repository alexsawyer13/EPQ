#include <Game/World.h>
#include <Game/WorldGeneration.h>
#include <Core/Core.h>
#include <Graphics/Shader.h>
#include <Maths/Maths.h>
#include <physics/Frustum.h>

#include <spdlog/spdlog.h>

void WorldLoadChunk(World *world, int x, int z);
void WorldUnloadChunk(World *world, int x, int z);

void WorldCreate(World *world)
{
	world->Chunks.reserve(1000); // TODO: TEMPORARY FIX
	WorldGenerationSetup(100u);

	for (int x = -RENDERDISTANCE; x <= RENDERDISTANCE; x++)
	{
		for (int z = -RENDERDISTANCE; z <= RENDERDISTANCE; z++)
		{
			Chunk chunk{ 0 };
			ChunkCreate(&chunk, x, z, world->Chunks.size());
			ChunkGenerate(&chunk);
			world->Chunks.push_back(chunk);
			world->ActiveChunks[CHUNK_HASH(x, z)] = world->Chunks.size() - 1;
			world->PendingMesh.push_back(world->Chunks.size() - 1);
		}
	}
}

void WorldDestroy(World *world)
{

}

void WorldDrawChunks(World *world, int width, int height)
{
	//Frustum frustum = CreateFrustumFromPlayer(&core.player, glm::radians(45.0F), (float)width/height, 0.1F, 1000.0f);

	for (auto iter = world->ActiveChunks.begin(); iter != world->ActiveChunks.end(); ++iter)
	{
		Chunk &chunk = world->Chunks[iter->second];
		if (chunk.Visible)
		{
			//if (FrustumCheckCuboid(frustum, (chunk.X+0.5f) * CHUNK_WIDTH, (float)CHUNK_HEIGHT * 0.5f, (chunk.Z+0.5f) * CHUNK_WIDTH, (float)CHUNK_WIDTH * 0.5f, (float)CHUNK_HEIGHT * 0.5f, (float)CHUNK_WIDTH * 0.5f))
			{
				core.shaders["optimisedtexarray"].SetMat4("u_Model", chunk.Model);
				VaoDraw(&world->Chunks[iter->second].Vao);
			}
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

Chunk *WorldGetChunkIfActive(World *world, int x, int z)
{
	ChunkCoord cc = { x, z };
	uint64_t hash = cc.Hash();

	world->ActiveChunks.find(hash);
	auto active = world->ActiveChunks.find(hash);
	if (active != world->ActiveChunks.end())
		return &world->Chunks[world->ActiveChunks[hash]];

	return nullptr;
}

Block &WorldGetBlock(World *world, int x, int y, int z)
{
	if (y > CHUNK_HEIGHT - 1 || y < 0)
	{
		//spdlog::error("Trying to access block that is above or below world, returning block id 0"); TODO: BETTER ERRORS
		return core.Blocks[0];
	}

	int chunkX = RoundToLowest((float)x / CHUNK_WIDTH);
	int chunkZ = RoundToLowest((float)z / CHUNK_WIDTH);

	int localX = x - CHUNK_WIDTH * chunkX;
	int localZ = z - CHUNK_WIDTH * chunkZ;

	auto iter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ));
	if (iter == world->ActiveChunks.end()) // Block isn't in active chunk
	{
		//spdlog::error("Trying to access block that is outside of active chunks, returning block id 0"); TODO: BETTER ERRORS
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

	WorldPushChunkMeshPriorityUpdate(world, iter->second);

	if (localX == CHUNK_WIDTH - 1)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX + 1, chunkZ));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshPriorityUpdate(world, niter->second);
		}
	}
	if (localX == 0)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX - 1, chunkZ));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshPriorityUpdate(world, niter->second);
		}
	}
	if (localZ == CHUNK_WIDTH - 1)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ + 1));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshPriorityUpdate(world, niter->second);
		}
	}
	if (localZ == 0)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ - 1));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshPriorityUpdate(world, niter->second);
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
			WorldPushChunkMeshPriorityUpdate(world, niter->second);
		}
	}
	if (localX == 0)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX - 1, chunkZ));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshPriorityUpdate(world, niter->second);
		}
	}
	if (localZ == CHUNK_WIDTH - 1)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ + 1));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshPriorityUpdate(world, niter->second);
		}
	}
	if (localZ == 0)
	{
		auto niter = world->ActiveChunks.find(CHUNK_HASH(chunkX, chunkZ - 1));
		if (niter != world->ActiveChunks.end()) // Found neighbouring chunk in active chunks
		{
			WorldPushChunkMeshPriorityUpdate(world, niter->second);
		}
	}

	WorldPushChunkMeshPriorityUpdate(world, iter->second);
}

Block &WorldGetBlock(World *world, float x, float y, float z)
{
	return WorldGetBlock(world, RoundToLowest(x), RoundToLowest(y), RoundToLowest(z));
}

void WorldUpdate(World *world)
{
	// Generate chunks if any are waiting to be generated
	for (int i = 0; i < ChunkMeshesPerFrame; i++)
	{
		if (world->PendingMesh.size() > 0)
		{
			Chunk &chunk = world->Chunks[world->PendingMesh.front()];
			world->PendingMesh.pop_front();

			if (!chunk.Generated) ChunkGenerate(&chunk);
			ChunkBuildMesh(&chunk);
		}
	}

	if (core.player.HasChangedChunk)
	{
		int oldChunkX = RoundToLowest((float)core.player.OldPosition.x / CHUNK_WIDTH);
		int oldChunkZ = RoundToLowest((float)core.player.OldPosition.z / CHUNK_WIDTH);
		int chunkX = RoundToLowest((float)core.player.Position.x / CHUNK_WIDTH);
		int chunkZ = RoundToLowest((float)core.player.Position.z / CHUNK_WIDTH);

		// Loop through chunks and see if they should be active still

		std::vector<std::pair<int, int>> chunksToUnload(10);

		for (auto iter = world->ActiveChunks.begin(); iter != world->ActiveChunks.end(); iter++)
		{
			Chunk *chunk = &world->Chunks[iter->second];
			int dx = abs(chunk->X - chunkX);
			int dz = abs(chunk->Z - chunkZ);
			if (dx <= RENDERDISTANCE && dz <= RENDERDISTANCE)
			{
				// Stay active
			}
			else
			{
				// Deactivate (can't do in the loop otherwise it invalidates iter)
				//WorldUnloadChunk(world, chunk->X, chunk->Z);
				chunksToUnload.emplace_back(chunk->X, chunk->Z);
			}
		}

		// Actually unload chunks
		for (auto pair : chunksToUnload)
		{
			WorldUnloadChunk(world, pair.first, pair.second);
		}

		// Loop through chunks and make sure they're active and generated if they should be

		for (int x = chunkX - RENDERDISTANCE; x <= chunkX + RENDERDISTANCE; x++)
		{
			for (int z = chunkZ - RENDERDISTANCE; z <= chunkZ + RENDERDISTANCE; z++)
			{
				WorldLoadChunk(world, x, z);
			}
		}
	}
}

void WorldLoadChunk(World *world, int x, int z)
{
	PROFILE_SCOPE_US("WorldLoadChunk");

	uint64_t hash = CHUNK_HASH(x, z);

	// If chunk is active skip
	if (world->ActiveChunks.find(hash) != world->ActiveChunks.end())
	{
		//spdlog::debug("Skipping chunk ({}, {})", x, z);
		return;
	}

	// If chunk is inactive make it active and build its mesh
	auto iter = world->InactiveChunks.find(hash);
	if (iter != world->InactiveChunks.end())
	{
		//spdlog::debug("Activating chunk ({}, {})", x, z);
		world->ActiveChunks[hash] = iter->second;
		WorldPushChunkMeshUpdate(world, iter->second);
		world->InactiveChunks.erase(hash);
		return;
	}

	// Otherwise make the chunk and make it active
	//spdlog::debug("Making chunk ({}, {})", x, z);
	Chunk chunk{ 0 };
	ChunkCreate(&chunk, x, z, world->Chunks.size()); // Don't generate chunk, let that happen when mesh building occurs
	world->Chunks.push_back(chunk);
	world->ActiveChunks[CHUNK_HASH(x, z)] = world->Chunks.size() - 1;
	WorldPushChunkMeshUpdate(world, world->Chunks.size() - 1);
}


void WorldUnloadChunk(World *world, int x, int z)
{
	PROFILE_SCOPE_US("WorldUnloadChunk");

	uint64_t hash = CHUNK_HASH(x, z);

	// If chunk is active, unload
	auto iter = world->ActiveChunks.find(hash);
	if (iter != world->ActiveChunks.end())
	{
		// TODO: A chunk could be unloaded while still in the pendingmesh queue
		ChunkDestroyMesh(&world->Chunks[iter->second]);
		world->InactiveChunks[hash] = iter->second;
		world->ActiveChunks.erase(iter->first);
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

void WorldPushChunkMeshPriorityUpdate(World *world, int chunk_id)
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
		world->PendingMesh.push_front(chunk_id);
	}
}