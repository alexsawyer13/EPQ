#include <Game/Chunk.h>
#include <Data/CubeData.h>
#include <Core/Timer.h>
#include <Core/Core.h>

#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>

void ChunkCreate(Chunk *chunk, int x, int z)
{
	chunk->X = x;
	chunk->Z = z;
	chunk->Model = glm::translate(glm::mat4(1.0f), glm::vec3(x * CHUNK_WIDTH, 0, z * CHUNK_WIDTH));

	chunk->Data = new uint16_t[CHUNK_VOLUME];

	uint16_t grass = BLOCK_PACK(core.BlockIds["grass"], 0);
	uint16_t dirt = BLOCK_PACK(core.BlockIds["dirt"], 0);
	uint16_t bedrock = BLOCK_PACK(core.BlockIds["bedrock"], 0);
	uint16_t stone = BLOCK_PACK(core.BlockIds["stone"], 0);
	uint16_t furnace_on = BLOCK_PACK(core.BlockIds["furnace_on"], 0);

	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int z = 0; z < CHUNK_WIDTH; z++)
			{
				if (y == 0)
					chunk->Data[CHUNK_INDEX_OF(x, y, z)] = bedrock;
				else if (y < 64)
					chunk->Data[CHUNK_INDEX_OF(x, y, z)] = stone;
				else if (y < 72)
					chunk->Data[CHUNK_INDEX_OF(x, y, z)] = dirt;
				else if (y == 72)
					chunk->Data[CHUNK_INDEX_OF(x, y, z)] = grass;
				else
					chunk->Data[CHUNK_INDEX_OF(x, y, z)] = 0;
			}
		}
	}

	//for (int i = 15; i > 0; i--)
	//	chunk->Data[CHUNK_INDEX_OF(0, i, 0)] = 0;

	//chunk->Data[CHUNK_INDEX_OF(10, CHUNK_HEIGHT - 1, 10)] = furnace_on;

	VaoCreate(&chunk->Vao);
	VboCreate(&chunk->Vbo);
	IboCreate(&chunk->Ibo);

	chunk->Visible = false;
}

void ChunkDestroy(Chunk *chunk)
{
	delete[] chunk->Data;
	VaoDestroy(&chunk->Vao);
	VboDestroy(&chunk->Vbo);
	IboDestroy(&chunk->Ibo);
}

void ChunkSetNeighbours(Chunk *chunk, Chunk *px, Chunk *nx, Chunk *pz, Chunk *nz)
{
	chunk->Posx = px;
	chunk->Negx = nx;
	chunk->Posz = pz;
	chunk->Negz = nz;
}

void ChunkBuildMesh(Chunk *chunk)
{
	//ScopeTimerUs timer("ChunkBuildMesh");

	std::vector<float> data;
	std::vector<unsigned int> indices;
	unsigned int currentIndex = 0;

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

						if (nx < 0)
						{
							if (chunk->Negx)
								isTransparent = ChunkGetBlock(chunk->Negx, CHUNK_WIDTH - 1, ny, nz).IsTransparent;
							else
								isTransparent = false;
						}
						else if (nx > CHUNK_WIDTH - 1)
						{
							if (chunk->Posx)
								isTransparent = ChunkGetBlock(chunk->Posx, 0, ny, nz).IsTransparent;
							else
								isTransparent = false;
						}
						else if (nz < 0)
						{
							if (chunk->Negz)
								isTransparent = ChunkGetBlock(chunk->Negz, nx, ny, CHUNK_WIDTH - 1).IsTransparent;
							else
								isTransparent = false;
						}
						else if (nz > CHUNK_WIDTH - 1)
						{
							if (chunk->Posz)
								isTransparent = ChunkGetBlock(chunk->Posz, nx, ny, 0).IsTransparent;
							else
								isTransparent = false;
						}
						else if (ny < 0 || ny > CHUNK_HEIGHT - 1)
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
	VaoAddVbo(&chunk->Vao, &chunk->Vbo, { { 3, GL_FLOAT }, { 1, GL_FLOAT }, {1, GL_FLOAT} });
	VaoSetIbo(&chunk->Vao, &chunk->Ibo);

	chunk->Visible = true;
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