#version 330 core

layout(location = 0) in vec3 vertexdata;
layout(location = 1) in vec2 textcoord;

out vec2 TexCoords;

uniform mat4 modelMat;

void main()
{
    gl_Position = modelMat * vec4(vertexdata.x, vertexdata.y, 0.0, 1.0); 
    TexCoords = textcoord;
}  


  