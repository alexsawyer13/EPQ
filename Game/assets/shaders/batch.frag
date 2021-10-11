#version 330 core

in vec2 v_TexCoord;
flat in unsigned int v_TexUnit;

out vec4 o_FragColour;

uniform sampler2D u_Samplers[32];

void main()
{
	o_FragColour = texture(u_Samplers[v_TexUnit], v_TexCoord);
	//o_FragColour = vec4(1.0, 0.0, 0.0, 1.0);
}