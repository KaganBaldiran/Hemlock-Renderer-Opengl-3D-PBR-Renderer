#version 330 core

layout(location = 0) in vec3 Position;


uniform mat4 model;
uniform mat4 cameramatrix;


void main()
{

  gl_Position = cameramatrix * model * vec4(Position , 1.0);

}