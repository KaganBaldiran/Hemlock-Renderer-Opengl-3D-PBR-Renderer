#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D Viewport;

void main()
{ 
    vec4 OutColor = texture(Viewport, TexCoords);
    FragColor = vec4(pow(OutColor.xyz,vec3(0.9)),OutColor.w);
}