#version 330 core

in vec2 v_TexCoord;
flat in uint v_Index;
flat in uint v_AnimationFrames;

out vec4 o_FragColour;

uniform sampler2DArray u_Texture;

uniform uint u_CurrentAnimationFrame;

void main()
{
	uint frame = u_CurrentAnimationFrame % v_AnimationFrames; // 0 to v_AnimationFrames - 1
	o_FragColour = texture(u_Texture, vec3(v_TexCoord, v_Index + frame));
}