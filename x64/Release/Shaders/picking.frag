#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D IDtexture;

void main()
{
	FragColor = vec4(vec3(texture(IDtexture,TexCoords).a),1.0f);
	//FragColor = texture(IDtexture,TexCoords);
}