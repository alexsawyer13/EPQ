#include <Graphics/Buffers.h>

#include <spdlog/spdlog.h>

// Vertex Buffers

void VboBind(VertexBuffer *vbo)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo->Handle);
}

void VboUnbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VboCreate(VertexBuffer *vbo)
{
	glGenBuffers(1, &(vbo->Handle));
}

void VboDestroy(VertexBuffer *vbo)
{
	if (vbo->Handle)
		glDeleteBuffers(1, &(vbo->Handle));
}

void VboData(VertexBuffer *vbo, GLsizeiptr size, const void *data, GLenum usage)
{
	VboBind(vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void VboSubData(VertexBuffer *vbo, GLintptr offset, GLsizeiptr size, const void *data)
{
	VboBind(vbo);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

// Index Buffers

void IboCreate(IndexBuffer *ibo)
{
	glGenBuffers(1, &(ibo->Handle));
}

void IboDestroy(IndexBuffer *ibo)
{
	if (ibo->Handle)
		glDeleteBuffers(1, &(ibo->Handle));
}

void IboBind(IndexBuffer *ibo)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->Handle);
}

void IboUnbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IboData(IndexBuffer *ibo, GLsizeiptr size, unsigned int *data, GLenum usage)
{
	IboBind(ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
	ibo->Count = size / sizeof(unsigned int);
}

// Vertex Arrays

void VaoCreate(VertexArray *vao)
{
	glGenVertexArrays(1, &(vao->Handle));
}

void VaoDestroy(VertexArray *vao)
{
	if (vao->Handle)
		glDeleteVertexArrays(1, &(vao->Handle));
}

void VaoBind(VertexArray *vao)
{
	glBindVertexArray(vao->Handle);
}

void VaoUnbind()
{
	glBindVertexArray(0);
}

void VaoAddVbo(VertexArray *vao, VertexBuffer *vbo, BufferLayout layout)
{
	VaoBind(vao);
	VboBind(vbo);

	for (int i = 0; i < layout.Layout.size(); i++)
	{
		auto &element = layout.Layout[i];

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.Count, element.Type, GL_FALSE, layout.Stride, (const void *)element.Offset);
	}
}

void VaoSetIbo(VertexArray *vao, IndexBuffer *ibo)
{
	VaoBind(vao);
	IboBind(ibo);
	vao->Ibo = ibo;
}

void VaoDraw(VertexArray *vao)
{
	VaoBind(vao);
	glDrawElements(GL_TRIANGLES, vao->Ibo->Count, GL_UNSIGNED_INT, 0);
}

void VaoDraw(VertexArray *vao, size_t count)
{
	VaoBind(vao);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

// Buffer Layouts

BufferLayoutElement::BufferLayoutElement(size_t count, GLenum type)
	: Count(count), Type(type)
{
	Size = GLTypeToSize(Type) * Count;
}

BufferLayout::BufferLayout(std::initializer_list<BufferLayoutElement> layout)
	: Layout(layout), Stride(0)
{
	for (auto &element : Layout)
	{
		element.Offset = Stride;
		Stride += element.Size;
	}
}

size_t GLTypeToSize(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:
		return sizeof(float);
	case GL_UNSIGNED_INT:
		return sizeof(unsigned int);
	case GL_INT:
		return sizeof(int);
	case GL_UNSIGNED_BYTE:
		return sizeof(unsigned char);
	case GL_BYTE:
		return sizeof(char);
	default:
		spdlog::error("Cannot convert GL type to size, defaulting to float");
		return sizeof(float);
	}
}