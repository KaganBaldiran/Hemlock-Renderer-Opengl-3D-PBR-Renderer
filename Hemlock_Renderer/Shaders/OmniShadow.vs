#version 330 core

layout(location = 0) in vec3 vertexdata;
layout(location = 3) in vec3 tangentnormal;
layout(location = 4) in vec3 bitangentnormal;
layout(location = 7) in vec3 inputcolors;
layout(location = 2) in vec2 textcoord;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;

void main()
{
   gl_Position = model * vec4(vertexdata , 1.0f);
}