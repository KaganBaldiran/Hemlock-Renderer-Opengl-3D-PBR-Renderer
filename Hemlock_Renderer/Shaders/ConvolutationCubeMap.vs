#version 330 core

layout (location = 0) in vec3 Vpos;

out vec3 localPos;

uniform mat4 proj;
uniform mat4 view;

void main()
{
  localPos = Vpos;
  gl_Position = proj * view * vec4(Vpos , 1.0);
}