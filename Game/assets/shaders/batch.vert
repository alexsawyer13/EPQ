#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in unsigned int a_TexUnit;

out vec2 v_TexCoord;
flat out unsigned int v_TexUnit;

uniform mat4 u_Proj;

void main()
{
	gl_Position = u_Proj * vec4(a_Position, 1.0);
	v_TexCoord = a_TexCoord;
	v_TexUnit = a_TexUnit;
}