#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
in mat4 modelMatr;

uniform sampler2D Viewport;
uniform sampler2D SSAO;
uniform int RenderPass;
uniform bool EnableSSAO;

uniform bool DOFenabled;
uniform float DOFfarDistance;
uniform float DOFintensity;
uniform sampler2D PositionBuffer;
uniform float FarPlane;
uniform float NearPlane;
uniform vec3 CameraPosition;
uniform mat4 ViewMatrix;

void main()
{ 
    if(RenderPass == 1)
    {
      vec4 OutColor;
      if(DOFenabled)
      {
	    float DeltaDistance = FarPlane - NearPlane;
        vec3 FragmentPosition = vec3(inverse(ViewMatrix) * vec4(texture(PositionBuffer, TexCoords).xyz,1.0f));
        float CamDistance = distance(CameraPosition,FragmentPosition)/DeltaDistance;

		if(CamDistance >= DOFfarDistance)
		{
			vec2 texelSize = DOFintensity / vec2(textureSize(Viewport,0));
			vec3 result = vec3(0.0f);
			for(int x = -2 ; x < 2 ; ++x)
			{
				for(int y = -2 ; y < 2 ; ++y)
				{
					vec2 offset = vec2(float(x) , float(y)) * texelSize;
					result += texture(Viewport,TexCoords + offset).xyz; 
				}
			}

			result = result / vec3(4.0f * 4.0f);
            OutColor = vec4(result,1.0f);
		}
		else
		{ 
		   OutColor = texture(Viewport, TexCoords);
		}
	}
    else
    { 
       OutColor = texture(Viewport, TexCoords);
	}

    if(EnableSSAO)
    {
        OutColor = OutColor * vec4(vec3(texture(SSAO,TexCoords).r),1.0f);
    }
    
    FragColor = vec4(pow(OutColor.xyz,vec3(0.9)),OutColor.w);

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