#version 330 core

  in vec2 finaltextcoord;
  in vec3 finalcolor;
  out vec4 outColor;
  in vec3 Normal;
  in vec3 currentpos;

  in vec3 bitangent;
  in vec3 tangent;

  in mat3 TBN;

  in vec4 FragPosLight;

  uniform sampler2D color;

  uniform vec4 colorpr;

  uniform sampler2D texture_diffuse0;
  uniform sampler2D texture_specular0;
  uniform sampler2D texture_normal0;
  uniform sampler2D texture_metalic0;
  uniform sampler2D texture_diffuse1;
  uniform sampler2D texture_specular1;
  uniform sampler2D texture_normal1;
  uniform sampler2D texture_metalic1;
  uniform sampler2D texture_diffuse2;
  uniform sampler2D stexture_specular2;
  uniform sampler2D texture_normal2;
  uniform sampler2D texture_metalic2;
  uniform sampler2D texture_diffuse3;
  uniform sampler2D texture_specular3;
  uniform sampler2D texture_normal3;
  uniform sampler2D texture_metalic3;
  uniform sampler2D texture_diffuse4;
  uniform sampler2D texture_specular4;
  uniform sampler2D texture_normal4;
  uniform sampler2D texture_metalic4;
  uniform sampler2D shadowMap;


  uniform vec4 lightColor1;
  uniform vec3 lightpos1;

  #define MAX_LIGHT_COUNT 20

  uniform vec4 lightColors[MAX_LIGHT_COUNT];
  uniform vec3 lightpositions[MAX_LIGHT_COUNT];
  uniform float lightIntensities[MAX_LIGHT_COUNT];
  uniform int numberoflights;

  uniform int typeoflight[MAX_LIGHT_COUNT];


  uniform int stencilindex;
  uniform int modelID;

  uniform bool enablehighlight;


  uniform int disableclaymaterial[4];


  uniform vec3 campos;


  uniform samplerCube skybox;
  in vec3 inverse_normal;


  float near = 0.1;
  float far = 100.0;

  const float PI = 3.14159265359;


  //Uniforms to control material
  uniform vec3 albedo;
  uniform float metallic;
  uniform float roughness;
  uniform float ao;

  uniform float LightIntensity;

  uniform float farPlane;

 
  float ShadowCalculation(vec4 fragPosLightSpace , vec3 lightDir ,vec3 normal)
  {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.003);  
    //float bias = 0.05;  
    
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
      for(int y = -1; y <= 1; ++y)
      {
         float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
         shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
      }    
    }
    shadow /= 9.0;


    return shadow;
  }

  float ShadowCalculationOmni(vec3 fragPos)
  {
      vec3 fragTolight = fragPos - lightpositions[0];
      float closestDepth = texture(skybox,fragTolight).r;
      closestDepth *= 25.0f;

      float currentDepth = length(fragTolight);

      float bias = 0.05;
      float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
      
      return shadow;
  }

  float DistributionGGX(vec3 N , vec3 H, float roughness)
  {
      float a = roughness * roughness;
      float a2 = a*a;
      float NdotH = max(dot(N,H),0.0);
      float NdotH2 = NdotH * NdotH;

      float num = a2;
      float denom = (NdotH2 * (a2 - 1.0) + 1.0);
      denom = PI * denom * denom;

      return num / denom;
  }

  float GeometrySchlickGGX(float NdotV , float roughness)
  {
      float r = (roughness + 1.0);
      float k = (r*r) / 8.0;

      float num = NdotV;
      float denom = NdotV * (1.0 - k) + k;

      return num / denom;
  }

  float GeometrySmith(vec3 N , vec3 V , vec3 L , float roughness)
  {
      float NdotV = max(dot(N,V),0.0);
      float NdotL = max(dot(N,L),0.0);
      float ggx2 = GeometrySchlickGGX(NdotV,roughness);
      float ggx1 = GeometrySchlickGGX(NdotL,roughness);

      return ggx1 * ggx2;
  }

  vec3 FresnelSchlick(float cosTheta , vec3 F0)
  {
      return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta,0.0,1.0),5.0);
  }

  
  void main() 
  {
     vec3 texturecolor;
     
     if(disableclaymaterial[0] == 0)
     {
        texturecolor = albedo;
     }
     else
     {
         texturecolor = pow(texture(texture_diffuse1, finaltextcoord).rgb,vec3(2.2));
     }

     float roughnessmap;

     if(disableclaymaterial[1] == 0)
     {
        roughnessmap = roughness;
     }
     else
     {
        roughnessmap = texture(texture_specular1, finaltextcoord).r;
     }

     vec3 resultnormal;

     if(disableclaymaterial[2] == 0)
     {
         resultnormal = Normal;
     }
     else
     {
          resultnormal = texture(texture_normal1,finaltextcoord).rgb;
          resultnormal = resultnormal * 2.0f - 1.0f;
          resultnormal = normalize(TBN * resultnormal);
     }


     float metalicmap;

     if(disableclaymaterial[3] == 0)
     {
        metalicmap = metallic;
     }
     else
     {
        metalicmap = texture(texture_metalic1, finaltextcoord).r;
     }

     float shadow;

     shadow = ShadowCalculationOmni(currentpos);

      vec3 N = normalize(resultnormal);
      vec3 V = normalize(campos - currentpos);

      vec3 F0 = vec3(0.04);
      F0 = mix(F0,texturecolor,metalicmap);

      vec3 Lo = vec3(0.0);
      for(int i = 0; i < numberoflights;++i)
      {
          vec3 L = normalize(lightpositions[i] - currentpos);
          vec3 H = normalize(V + L);
          float distance = length(lightpositions[i] - currentpos);
          float attenuation = 1.0 / (distance * distance);
          vec3 radiance = lightColors[i].xyz * attenuation;

          float NDF = DistributionGGX(N,H,roughnessmap);
          float G = GeometrySmith(N,V,L,roughnessmap);
          vec3 F = FresnelSchlick(max(dot(H,V),0.0),F0);

          vec3 kS = F;
          vec3 Kd = vec3(1.0) - kS;
          Kd *= 1.0 - metalicmap;

          vec3 numerator = NDF * G * F;
          float denominator = 4.0 * max(dot(N,V),0.0) * max(dot(N,L),0.0) + 0.0001;
          vec3 specular = numerator / denominator;

          float NdotL = max(dot(N,L),0.0);

          if(i == 0)
          {
               
              Lo += (1.0 - shadow) * (Kd * texturecolor / PI + specular) * radiance * lightIntensities[i] * NdotL;
          }
          else
          {
              Lo += (Kd * texturecolor / PI + specular) * radiance * lightIntensities[i] * NdotL;
          }
      }
     
      vec3 ambient = vec3(0.03) * texturecolor * ao;
      vec3 color = ambient + Lo;

      color = color / (color + vec3(1.0));
      color = pow(color , vec3(1.0/2.2));

      outColor = vec4(color , 1.0);
  }