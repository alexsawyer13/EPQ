#version 330 core

layout (location = 0) in vec3 a_Pos;

out vec3 TexCoord;

uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
	gl_Position = u_Proj * u_View * vec4(a_Pos, 1.0);
	TexCoord = a_Pos;
}