#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
in mat4 modelMatr;

uniform sampler2D Viewport;
uniform sampler2D SSAO;
uniform int RenderPass;
uniform bool EnableSSAO;

void main()
{ 
    if(RenderPass == 1)
    {
      if(EnableSSAO)
      {
        vec4 OutColor = texture(Viewport, TexCoords) * vec4(vec3(texture(SSAO,TexCoords).r),1.0f);
        FragColor = vec4(pow(OutColor.xyz,vec3(0.9)),OutColor.w);
      }
      else
      {
        vec4 OutColor = texture(Viewport, TexCoords);
        FragColor = vec4(pow(OutColor.xyz,vec3(0.9)),OutColor.w);
      }
    }
    else if(RenderPass == 3)
    {
      vec4 OutColor = texture(Viewport, TexCoords);
      FragColor = vec4(pow(OutColor.xyz,vec3(0.9)),1.0f);
    }
    else if(RenderPass == 5)
    {
      vec4 OutColor = texture(Viewport, TexCoords);
      FragColor = vec4(pow(OutColor.xyz/10.0f,vec3(0.9)),OutColor.w);
    }
    else if(RenderPass == 6)
    {
      vec4 OutColor = texture(Viewport, TexCoords);
      FragColor = vec4(pow(vec3(OutColor.x),vec3(0.9)),1.0f);
    }
    else if(RenderPass == 7)
    {
      vec4 OutColor = texture(Viewport, TexCoords);
      FragColor = vec4(pow(vec3(OutColor.w),vec3(0.9)),1.0f);
    }
    else
    {
      vec4 OutColor = texture(Viewport, TexCoords);
      FragColor = vec4(pow(OutColor.xyz,vec3(0.9)),OutColor.w);
    }
}