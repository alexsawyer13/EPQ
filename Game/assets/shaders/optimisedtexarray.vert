#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in float a_TextureValue; // 0 1 2 3 are the corners of texture array index 0, 4 5 6 7 are the corners of texture array index 1, etc.
layout (location = 2) in float a_AnimationFrames;

out vec2 v_TexCoord;
flat out uint v_Index;
flat out uint v_AnimationFrames;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

vec2 texCoords[4] = vec2[4](
    vec2(1.0f, 1.0f), // 0 Top right
    vec2(0.0f, 1.0f), // 1 Top left
    vec2(1.0f, 0.0f), // 2 Bottom right
    vec2(0.0f, 0.0f)  // 3 Bottom left
);

void main()
{
    gl_Position = u_Proj * u_View * u_Model * vec4(a_Pos, 1.0);
    
    uint texValue = uint(a_TextureValue);

    v_TexCoord = texCoords[int(texValue) % 4];
    v_Index = uint(texValue / 4u);

    v_AnimationFrames = uint(a_AnimationFrames);
}