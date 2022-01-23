#pragma once

#include <Graphics/Buffers.h>
#include <Data/Blocks.h>

#include <glm/glm.hpp>

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_AREA = (CHUNK_WIDTH * CHUNK_WIDTH);
constexpr int CHUNK_VOLUME = (CHUNK_AREA * CHUNK_HEIGHT);

#define CHUNK_INDEX_OF(x, y, z) ((y) * CHUNK_AREA + (z) * CHUNK_WIDTH + (x))

/*
uint16_t is a 2 byte data structure
the first 10 bits is the block id
the next 6 bits can be used for various other data (rotations or something)
*/

#define BLOCK_ID(x) (x >> 6)
#define BLOCK_META(x) (x & 0x3f)
#define BLOCK_PACK(id, meta) ((id << 6) + (meta & 0x3f))
#define BLOCK_UNPACK(x, id, meta) id = BLOCK_ID(x); meta = BLOCK_META(x)

struct Chunk
{
	uint16_t *Data;

	VertexArray Vao;
	VertexBuffer Vbo;
	IndexBuffer Ibo;

	int Posx, Negx, Posz, Negz;

	int X, Z, Index;
	glm::mat4 Model;

	bool Visible;
};

void ChunkCreate(Chunk *chunk, int x, int z, int index);
void ChunkDestroy(Chunk *chunk);

void ChunkBuildMesh(Chunk *chunk);
void ChunkDestroyMesh(Chunk *chunk);

uint16_t ChunkGetBlockId(Chunk *chunk, int x, int y, int z);
Block &ChunkGetBlock(Chunk *chunk, int x, int y, int z);