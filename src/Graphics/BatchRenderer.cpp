#include <Graphics/BatchRenderer.h>
#include <Core/Core.h>

#include <unordered_map>
#include <array>

/*
static loat rectangleVertices[] = {
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Top right
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Top left
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom right
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Bottom left
};

unsigned int rectangleIndices[] = {
	0, 1, 2,
	2, 1, 3,
};
*/


void BatchCreate(BatchRenderer *renderer)
{
	CoreShaderBind("batch");
	for (int i = 0; i < MAX_TEXTURE_UNITS; i++)
	{
		char buffer[256];
		sprintf_s(buffer, "u_Samplers[%d]", i);
		CoreShaderSetInt("batch", buffer, i);
	}

	VaoCreate(&renderer->Vao);
	VboCreate(&renderer->Vbo);
	IboCreate(&renderer->Ibo);

	VboData(&renderer->Vbo, VERTEX_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);

	std::array<unsigned int, INDEX_BUFFER_COUNT> indices;
	for (int i = 0; i < MAX_QUADS; i++)
	{
		indices[0 + 6 * i] = 0 + 4 * i;
		indices[1 + 6 * i] = 1 + 4 * i;
		indices[2 + 6 * i] = 2 + 4 * i;
		indices[3 + 6 * i] = 2 + 4 * i;
		indices[4 + 6 * i] = 1 + 4 * i;
		indices[5 + 6 * i] = 3 + 4 * i;
	}

	IboData(&renderer->Ibo, INDEX_BUFFER_SIZE, &indices[0], GL_DYNAMIC_DRAW);

	VaoAddVbo(&renderer->Vao, &renderer->Vbo, BufferLayout{ {3, GL_FLOAT}, {2, GL_FLOAT}, {1, GL_FLOAT} });
	VaoSetIbo(&renderer->Vao, &renderer->Ibo);
}

void BatchDestroy(BatchRenderer *renderer)
{
	VaoDestroy(&renderer->Vao);
	VboDestroy(&renderer->Vbo);
	IboDestroy(&renderer->Ibo);
}

void BatchRender(BatchRenderer *renderer)
{
	CoreShaderBind("batch");

	// Sort quads by texture
	std::unordered_map<Texture2D *, std::vector<Quad*>> quadsByTexture;

	for (auto &quad : renderer->Quads)
	{
		if (quadsByTexture.find(quad.Texture) == quadsByTexture.end())
		{
			quadsByTexture[quad.Texture] = std::vector<Quad*>();
		}
		quadsByTexture[quad.Texture].push_back(&quad);
	}

	// Go through each group of MAX_TEXTURE_UNITS textures and render them

	size_t numTextures = quadsByTexture.size();
	size_t numDraws = numTextures / MAX_TEXTURE_UNITS + 1; // Integer division rounds down
	size_t remainder = numTextures - (numDraws - 1) * MAX_TEXTURE_UNITS;

	auto iter = quadsByTexture.begin();	
	std::vector<float> data;
	data.reserve(MAX_QUADS * 6);

	for (size_t i = 0; i < numDraws; i++)
	{
		// If this is the last draw we only need to bind remainder textures, otherwise bind MAX_TEXTURE_UNITS
		size_t numTexturesToBind = (i == (numDraws - 1)) ? remainder : MAX_TEXTURE_UNITS;
		size_t numQuads = 0;

		// Bind textures in the correct slots
		for (unsigned int j = 0; j < numTexturesToBind; j++)
		{
			Texture2DBind(iter->first, j);

			// Add the quads to the buffer, flush if full
			for (auto &quad : iter->second)
			{
				// If the quad can't fit in the buffer, flush it
				if (numQuads == MAX_QUADS)
				{
					VboSubData(&renderer->Vbo, 0, data.size() * VERTEX_SIZE, data.data());
					data.clear();
					VaoDraw(&renderer->Vao, numQuads * 6);
					numQuads = 0;
				}

				// Top right
				data.push_back(quad->Position.x + quad->Size.x);
				data.push_back(quad->Position.y + quad->Size.y);
				data.push_back(quad->Position.z);
				data.push_back(1.0f);
				data.push_back(1.0f);
				data.push_back(float(j));

				// Top left
				data.push_back(quad->Position.x);
				data.push_back(quad->Position.y + quad->Size.y);
				data.push_back(quad->Position.z);
				data.push_back(0.0f);
				data.push_back(1.0f);
				data.push_back(float(j));

				// Bottom right
				data.push_back(quad->Position.x + quad->Size.x);
				data.push_back(quad->Position.y);
				data.push_back(quad->Position.z);
				data.push_back(1.0f);
				data.push_back(0.0f);
				data.push_back(float(j));

				// Bottom left
				data.push_back(quad->Position.x);
				data.push_back(quad->Position.y);
				data.push_back(quad->Position.z);
				data.push_back(0.0f);
				data.push_back(0.0f);
				data.push_back(float(j));

				numQuads++;
			}

			iter++;
		}

		// Flush any remaining quads

		// TODO: TEMP

		if (numQuads > 0)
		{
			VboSubData(&renderer->Vbo, 0, data.size() * VERTEX_SIZE, data.data());
			data.clear();
			VaoDraw(&renderer->Vao, numQuads * 6);
			numQuads = 0;
		}
	}
}

void BatchClear(BatchRenderer *renderer)
{
	renderer->Quads.clear();
}

void BatchFlush(BatchRenderer *renderer)
{
	BatchRender(renderer);
	BatchClear(renderer);
}