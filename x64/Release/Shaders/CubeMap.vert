#version 330 core

layout(location = 0) in vec3 Position;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 ScreenRatio;


void main()
{

  TexCoords = Position;
  vec4 pos = ScreenRatio * projection * view * vec4(Position, 1.0);
  gl_Position = pos.xyww;


}