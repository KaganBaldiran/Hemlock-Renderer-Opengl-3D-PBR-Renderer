#version 330 core

out vec4 FragColor;

uniform sampler2D IDtexture;
uniform float modelID;
uniform float RenderStep;

void main()
{
	FragColor = vec4(vec3(modelID),1.0f);  
}