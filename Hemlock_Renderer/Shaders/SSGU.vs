#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out mat4 modelMatr;

uniform mat4 modelMat;

void main()
{
    modelMatr = modelMat;
    gl_Position = modelMat * vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
}  