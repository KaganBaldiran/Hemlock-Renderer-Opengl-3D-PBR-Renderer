#version 330 core

layout (location = 0) out vec4 OutColor;
in vec2 TexCoords;

#define MAX_LIGHT_COUNT 20
uniform sampler2D SSLSbuffer;

uniform vec3 lightpositions[MAX_LIGHT_COUNT];
uniform float lightIntensities[MAX_LIGHT_COUNT];
uniform int numberoflights;
uniform mat4 ViewMat;

const float exposure = 0.3f;
const float decay = 0.96815;
const float density  = 0.926;
const float weight  = 0.587;
const int NUM_SAMPLES = 200;

vec4 CalculateRays(vec3 LightPos , float LightIntesity)
{
   vec2 tc = TexCoords.xy;
   vec4 LP = ViewMat * vec4(LightPos , 1.0f);
   vec3 screenLightPos = LP.xyz / LP.w;

   vec2 L = 0.5 * (screenLightPos.xy + 1.0);

   vec2 DeltaTexCoord = (tc - L);
   //vec2 DeltaTexCoord = (tc - (LightPos.xy * 0.5f + 0.5f));
   DeltaTexCoord *= 1.0f / float(NUM_SAMPLES);  
   float IlluminationDecay = 1.0f;

   vec4 RayColor = texture(SSLSbuffer, tc.xy)*0.4;

   for(int i = 0 ; i< NUM_SAMPLES ; i++) 
   {
      tc -= DeltaTexCoord;
      vec4 samp = texture(SSLSbuffer, tc.xy) * 0.4f;
      samp *= IlluminationDecay * weight; 
      RayColor += samp;
      IlluminationDecay *= decay;
   }

   return normalize(LightIntesity) * RayColor;
}

void main()
{
  vec4 result = vec4(0.0f);
  for(int i = 0 ; i < 1 ; i++) 
  {
      result += CalculateRays(lightpositions[i] , lightIntensities[i]);
  }
  result /= numberoflights;
  OutColor = vec4(result.xyz,1.0f);
}