#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in float a_Index;

out vec2 v_TexCoord;
flat out uint v_Index;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
    gl_Position = u_Proj * u_View * u_Model * vec4(a_Pos, 1.0);
    v_TexCoord = a_TexCoord;
    v_Index = uint(a_Index);
}