#version 330 core

layout (location = 0) in vec3 a_Pos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
    gl_Position = u_Proj * u_View * u_Model * vec4(a_Pos, 1.0);
}