#pragma once

#include <glm/glm.hpp>

#include <Graphics/Texture.h>
#include <Graphics/Buffers.h>

#include <vector>

/*
Vertex
  - Position - 3 floats - 12 bytes
  - TexCoords - 2 floats - 8 bytes
  - TextureID - 1 unsigned int - 4 bytes
  24 bytes per vertex
*/

constexpr size_t MAX_QUADS = 10000;

constexpr size_t VERTEX_SIZE = 3 * sizeof(float) + 2 * sizeof(float) + sizeof(unsigned int);
constexpr size_t VERTEX_BUFFER_COUNT = MAX_QUADS * 4;
constexpr size_t VERTEX_BUFFER_SIZE = VERTEX_SIZE * VERTEX_BUFFER_COUNT;

constexpr size_t INDEX_BUFFER_COUNT = MAX_QUADS * 6;
constexpr size_t INDEX_BUFFER_SIZE = sizeof(unsigned int) * INDEX_BUFFER_COUNT;

// TODO: Make this dynamic
constexpr size_t MAX_TEXTURE_UNITS = 16; // Guaranteed to be at least 16 by OpenGL

struct Quad
{
	glm::vec3 Position; // Z component for depth // Bottom left corner in pixels
	glm::vec2 Size;
	Texture2D *Texture;
};

struct BatchRenderer
{
	std::vector<Quad> Quads;

	VertexArray Vao;
	VertexBuffer Vbo;
	IndexBuffer Ibo;
};

void BatchCreate(BatchRenderer *renderer);
void BatchDestroy(BatchRenderer *renderer);
void BatchRender(BatchRenderer *renderer);
void BatchClear(BatchRenderer *renderer);
void BatchFlush(BatchRenderer *renderer);