#version 330 core


uniform uint objectindex;
uniform uint drawindex;


out uvec3 FragColor;


void main()
{
  FragColor = uvec3(objectindex,drawindex,gl_PrimitiveID);
}