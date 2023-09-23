#include <Data/CubeData.h>

namespace CubeData
{
	glm::vec3 Vertices[8] =
	{
		glm::vec3(0.0f, 0.0f, 1.0f), // Left  bottom front
		glm::vec3(1.0f, 0.0f, 1.0f), // Right bottom front
		glm::vec3(1.0f, 1.0f, 1.0f), // Right top    front
		glm::vec3(0.0f, 1.0f, 1.0f), // Left  top    front
		glm::vec3(0.0f, 0.0f, 0.0f), // Left  bottom back
		glm::vec3(1.0f, 0.0f, 0.0f), // Right bottom back
		glm::vec3(1.0f, 1.0f, 0.0f), // Right top    back
		glm::vec3(0.0f, 1.0f, 0.0f), // Left  top    back
	};

	glm::vec3 Normals[6] =
	{
		glm::vec3(0.0f,  1.0f,  0.0f), // Top face
		glm::vec3(0.0f, -1.0f,  0.0f), // Bottom face
		glm::vec3(-1.0f,  0.0f,  0.0f), // Left face
		glm::vec3(1.0f,  0.0f,  0.0f),  // Right face
		glm::vec3(0.0f,  0.0f,  1.0f), // Front face
		glm::vec3(0.0f,  0.0f, -1.0f), // Back face
	};

	unsigned int Faces[24] =
	{
		6, 7, 2, 3, // Top
		1, 0, 5, 4, // Bottom
		3, 7, 0, 4, // Left
		6, 2, 5, 1, // Right
		2, 3, 1, 0, // Front
		7, 6, 4, 5, // Back
	};

	float CubeVertices[108] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int RectangleIndices[] = {
		0, 1, 2,
		2, 1, 3,
	};
}