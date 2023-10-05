#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gColorSpec;

in vec3 currentpos;
in vec2 finaltextcoord;
in vec3 Normal; 

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float modelID;

void main()
{
	
   gPosition = vec4(currentpos,modelID);
   gNormal = vec4(normalize(Normal),1.0f);
   gColorSpec.rgb = texture(texture_diffuse1,finaltextcoord).rgb;
   gColorSpec.a = texture(texture_specular1,finaltextcoord).r;
	
}