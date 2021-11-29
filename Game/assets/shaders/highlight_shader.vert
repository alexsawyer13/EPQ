#version 330 core

layout (location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
	gl_Position = u_Proj * u_View * vec4(a_Position, 1.0);
}