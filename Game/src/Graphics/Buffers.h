#pragma once

#include <glad/glad.h>

#include <vector>
#include <initializer_list>

struct VertexBuffer
{
	GLuint Handle;
};

struct IndexBuffer
{
	GLuint Handle;
	size_t Count;
};

struct VertexArray
{
	GLuint Handle;
	size_t IboCount;
};

struct BufferLayoutElement
{
	size_t Count;
	GLenum Type;

	size_t Size;
	size_t Offset;

	BufferLayoutElement(size_t count, GLenum type);
};

struct BufferLayout
{
	std::vector<BufferLayoutElement> Layout;
	size_t Stride;

	BufferLayout(std::initializer_list<BufferLayoutElement> layout);
};

size_t GLTypeToSize(GLenum type);

void VboBind(VertexBuffer *vbo);
void VboUnbind();
void VboCreate(VertexBuffer *vbo);
void VboDestroy(VertexBuffer *vbo);
void VboData(VertexBuffer *vbo, GLsizeiptr size, const void *data, GLenum usage);
void VboSubData(VertexBuffer *vbo, GLintptr offset, GLsizeiptr size, const void *data);

void IboCreate(IndexBuffer *ibo);
void IboDestroy(IndexBuffer *ibo);
void IboBind(IndexBuffer *ibo);
void IboUnbind();
void IboData(IndexBuffer *ibo, GLsizeiptr size, unsigned int *data, GLenum usage);

void VaoCreate(VertexArray *vao);
void VaoDestroy(VertexArray *vao);
void VaoBind(VertexArray *vao);
void VaoUnbind();
void VaoAddVbo(VertexArray *vao, VertexBuffer *vbo, BufferLayout layout);
void VaoSetIbo(VertexArray *vao, IndexBuffer *ibo);
void VaoDraw(VertexArray *vao);
void VaoDraw(VertexArray *vao, size_t count);