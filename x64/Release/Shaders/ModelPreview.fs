#version 330 core

  out vec4 outColor;

  in vec2 finaltextcoord;
  in vec3 finalcolor;
  in vec3 Normal;
  in vec3 currentpos;
  in vec3 bitangent;
  in vec3 tangent;
  in mat3 TBN;
  in vec4 FragPosLight;

  uniform vec3 CamPos;
  uniform vec3 ObjectScales;
  uniform float FarNearPlaneDifference;

   vec3 DirectionalLight(vec3 LightPosition , vec3 ObjectColor , vec3 LightColor , float LightIntensity)
  {
    float Ambient = 0.10f;

    vec3 specularColor = LightColor;

    vec3 N = Normal;
    vec3 L = LightPosition - currentpos;
    vec3 LDR = normalize(L);

    //float LightDistance = length(L);
    //float a = 0.2f;
    //float b = 0.1f;
    //float intensity = LightIntensity / (a * LightDistance * LightDistance + b * LightDistance + 1.0f);
   
    float intensity = LightIntensity;

    vec3 V = CamPos - currentpos;
    vec3 H = normalize(L + V);

    float diffuse = max(dot(N,LDR),0.0f);
    vec3 specular = pow(max(dot(N,H),0.0f),32.0f) * specularColor;

    return ObjectColor * LightColor * ((diffuse  * intensity + Ambient) + specular * intensity);
  }

  vec3 PointLight(vec3 LightPosition , vec3 ObjectColor , vec3 LightColor , float LightIntensity)
  {
    float Ambient = 0.10f;

    vec3 specularColor = LightColor;

    vec3 N = Normal;
    vec3 L = LightPosition - currentpos;
    vec3 LDR = normalize(L);

    float LightDistance = length(L);
    float a = 0.2f;
    float b = 0.1f;
    float intensity = LightIntensity / (a * LightDistance * LightDistance + b * LightDistance + 1.0f);
   
    vec3 V = CamPos - currentpos;
    vec3 H = normalize(L + V);

    float diffuse = max(dot(N,LDR),0.0f);
    vec3 specular = pow(max(dot(N,H),0.0f),32.0f) * specularColor;

    return ObjectColor * LightColor * ((diffuse  * intensity + Ambient) + specular * intensity);
  }

  void main()
  {

    float AverageScale = (ObjectScales.x + ObjectScales.y + ObjectScales.z) / 3.0f;
    vec3 LightPositions[3];
    LightPositions[0] = vec3(0.0f,ObjectScales.y * 2.0f,0.0f);
    LightPositions[1] = vec3(0.0f,0.0f,ObjectScales.z * 2.0f);
    LightPositions[2] = vec3(ObjectScales.x * 2.0f,0.0f,0.0f);
    
    vec3 ObjectColor = vec3(1.0f,1.0f,1.0f);
    vec3 LightColor0 = vec3(1.0f,1.0f,1.0f);

    float LightIntensity0 = 0.17f;

    vec3 Result = vec3(0.0f,0.0f,0.0f);
    for(int i = 0;i < 3;i++)
    {
       Result += DirectionalLight(LightPositions[i],ObjectColor,LightColor0,LightIntensity0);
    }

    outColor = vec4(Result,1.0f);
  }