#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 3) out vec4 SSLS;

uniform vec4 lightColor;

void main()
{
	FragColor = lightColor;
	SSLS = lightColor;
}