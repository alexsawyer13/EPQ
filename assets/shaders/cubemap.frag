#version 330 core

in vec3 TexCoord;

out vec4 FragColour;

uniform samplerCube u_Cubemap;

void main()
{
	FragColour = texture(u_Cubemap, TexCoord);
}