#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
in mat4 modelMatr;

uniform sampler2D Viewport;
uniform int RenderPass;

void main()
{ 
    if(RenderPass == 5)
    {
      vec4 OutColor = texture(Viewport, TexCoords);
      FragColor = vec4(pow(OutColor.xyz/10.0f,vec3(0.9)),OutColor.w);
    }
    else
    {
      vec4 OutColor = texture(Viewport, TexCoords);
      FragColor = vec4(pow(OutColor.xyz,vec3(0.9)),OutColor.w);
    }
}