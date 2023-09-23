#version 330 core

in vec2 v_TexCoord;
flat in uint v_Index;

out vec4 o_FragColour;

uniform sampler2DArray u_Texture;

void main()
{
	o_FragColour = texture(u_Texture, vec3(v_TexCoord, v_Index));
}