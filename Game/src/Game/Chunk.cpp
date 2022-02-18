#include <Game/Chunk.h>
#include <Game/WorldGeneration.h>
#include <Data/CubeData.h>
#include <Core/Profiler.h>
#include <Core/Core.h>

#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>

void ChunkCreate(Chunk *chunk, int x, int z, int index)
{
	PROFILE_SCOPE_US("ChunkCreate");

	chunk->X = x;
	chunk->Z = z;
	chunk->Index = index;
	chunk->Model = glm::translate(glm::mat4(1.0f), glm::vec3(x * CHUNK_WIDTH, 0, z * CHUNK_WIDTH));

	chunk->Data = new uint16_t[CHUNK_VOLUME];

	VaoCreate(&chunk->Vao);
	VboCreate(&chunk->Vbo);
	IboCreate(&chunk->Ibo);

	chunk->Visible = false;
	chunk->Generated = false;
}

void ChunkGenerate(Chunk *chunk)
{
	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int z = 0; z < CHUNK_WIDTH; z++)
			{
				chunk->Data[CHUNK_INDEX_OF(x, y, z)] = GenerateVoxel(x + chunk->X * CHUNK_WIDTH, y, z + chunk->Z * CHUNK_WIDTH);
			}
		}
	}
	chunk->Generated = true;
}

void ChunkDestroy(Chunk *chunk)
{
	delete[] chunk->Data;
	VaoDestroy(&chunk->Vao);
	VboDestroy(&chunk->Vbo);
	IboDestroy(&chunk->Ibo);
}

void ChunkBuildMesh(Chunk *chunk)
{
	PROFILE_SCOPE_US("ChunkBuildMesh");

	std::vector<float> data;
	std::vector<unsigned int> indices;
	unsigned int currentIndex = 0;

	// Get neighbours
	Chunk *Posx = WorldGetChunk(&core.world, chunk->X + 1, chunk->Z);
	Chunk *Negx = WorldGetChunk(&core.world, chunk->X - 1, chunk->Z);
	Chunk *Posz = WorldGetChunk(&core.world, chunk->X, chunk->Z + 1);
	Chunk *Negz = WorldGetChunk(&core.world, chunk->X, chunk->Z - 1);

	// Loop over every block
	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int z = 0; z < CHUNK_WIDTH; z++)
			{
				// Get info about the block
				uint32_t id, meta;
				BLOCK_UNPACK(chunk->Data[CHUNK_INDEX_OF(x, y, z)], id, meta);
				Block &block = core.Blocks[id];

				// Ignore non-blocks (air and stuff)
				if (block.IsBlock)
				{
					// Loop over faces
					for (int i = 0; i < 6; i++)
					{
						BlockTexture &texture = core.BlockTextures[block.Faces[i]];

						// Checks adjacent face to see if it's transparent
						bool isTransparent = false;
						int nx, ny, nz;
						nx = x + CubeData::Normals[i].x;
						ny = y + CubeData::Normals[i].y;
						nz = z + CubeData::Normals[i].z;

						// TODO: FIX THIS
						//if (nx < 0)
						//{
						//	if (Negx)
						//		isTransparent = ChunkGetBlock(Negx, CHUNK_WIDTH - 1, ny, nz).IsTransparent;
						//	else
						//		isTransparent = false;
						//}
						//else if (nx > CHUNK_WIDTH - 1)
						//{
						//	if (Posx)
						//		isTransparent = ChunkGetBlock(Posx, 0, ny, nz).IsTransparent;
						//	else
						//		isTransparent = false;
						//}
						//else if (nz < 0)
						//{
						//	if (Negz)
						//		isTransparent = ChunkGetBlock(Negz, nx, ny, CHUNK_WIDTH - 1).IsTransparent;
						//	else
						//		isTransparent = false;
						//}
						//else if (nz > CHUNK_WIDTH - 1)
						//{
						//	if (Posz)
						//		isTransparent = ChunkGetBlock(Posz, nx, ny, 0).IsTransparent;
						//	else
						//		isTransparent = false;
						//}
						//else if (ny < 0 || ny > CHUNK_HEIGHT - 1)
						//{
						//	isTransparent = true;
						//}
						if (nx < 0 || nx > CHUNK_WIDTH - 1 || nz < 0 || nz > CHUNK_WIDTH - 1 || ny < 0 || ny > CHUNK_HEIGHT - 1)
						{
							isTransparent = true;
						}
						else
						{
							isTransparent = ChunkGetBlock(chunk, nx, ny, nz).IsTransparent;
						}

						if (isTransparent)
						{
							// Loop over each corner
							for (int j = 0; j < 4; j++)
							{
								// Get x y z positions from CubeData and add them to vbo data
								data.push_back(x + CubeData::Vertices[CubeData::Faces[4 * i + j]].x);
								data.push_back(y + CubeData::Vertices[CubeData::Faces[4 * i + j]].y);
								data.push_back(z + CubeData::Vertices[CubeData::Faces[4 * i + j]].z);
								// Calculate the texture index as detailed in optimisedtexarray.vert (kind of)
								data.push_back(block.Faces[i] * 4 + j);
								// Add the number of animation frames
								data.push_back((float)(texture.AnimationFrames));
								// Add shading depending on direction
								if (CubeData::Normals[i].x == 0.0f && CubeData::Normals[i].y == 0.0f)
								{
									data.push_back(0.8f);
								}
								else if (CubeData::Normals[i].z == 0.0f && CubeData::Normals[i].y == 0.0f)
								{
									data.push_back(0.9f);
								}
								else if (CubeData::Normals[i].x == 0.0f && CubeData::Normals[i].z == 0.0f)
								{
									data.push_back(1.0f);
								}
							}
							// Add the indices for both of the triangles
							indices.push_back(currentIndex);
							indices.push_back(currentIndex + 1);
							indices.push_back(currentIndex + 2);
							indices.push_back(currentIndex + 2);
							indices.push_back(currentIndex + 1);
							indices.push_back(currentIndex + 3);
							// Increment the index for the next block
							currentIndex += 4;
						}
					}
				}
			}
		}
	}

	// Creates the OpenGL objects
	VboData(&chunk->Vbo, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	IboData(&chunk->Ibo, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	VaoAddVbo(&chunk->Vao, &chunk->Vbo, { { 3, GL_FLOAT }, { 1, GL_FLOAT }, {1, GL_FLOAT}, {1, GL_FLOAT} });
	VaoSetIbo(&chunk->Vao, &chunk->Ibo);

	chunk->Visible = true;
}

void ChunkDestroyMesh(Chunk *chunk)
{
	glInvalidateBufferData(chunk->Vbo.Handle);
	glInvalidateBufferData(chunk->Ibo.Handle);
	chunk->Visible = false;
}

uint16_t ChunkGetBlockId(Chunk *chunk, int x, int y, int z)
{
	if (x < 0 || x > CHUNK_WIDTH - 1 || y < 0 || y > CHUNK_HEIGHT - 1 || z < 0 || z > CHUNK_WIDTH - 1)
	{
		spdlog::error("Trying to access local block ({}, {}, {}) that is out of bounds of chunk ({}, {})", x, y, z, chunk->X, chunk->Z);
		throw;
	}

	return BLOCK_ID(chunk->Data[CHUNK_INDEX_OF(x, y, z)]);
}

Block &ChunkGetBlock(Chunk *chunk, int x, int y, int z)
{
	return core.Blocks[ChunkGetBlockId(chunk, x, y, z)];
}