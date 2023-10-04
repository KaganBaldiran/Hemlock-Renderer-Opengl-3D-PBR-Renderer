#version 330 core

layout (location = 0) in vec3 Vpos;

out vec3 LocalPos;

uniform mat4 ProjectionMat;
uniform mat4 ViewMat;

void main()
{
  LocalPos = Vpos;
  gl_Position = ProjectionMat * ViewMat * vec4(Vpos,1.0f);
}