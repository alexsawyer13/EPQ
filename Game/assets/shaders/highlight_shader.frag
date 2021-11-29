#version 330 core

out vec4 o_FragColour;

uniform vec4 u_Colour;

void main()
{
	o_FragColour = u_Colour;
}