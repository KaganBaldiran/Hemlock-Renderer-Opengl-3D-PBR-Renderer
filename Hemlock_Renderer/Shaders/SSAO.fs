#version 330 core

out float FragColor;

in vec2 TexCoords;

uniform sampler2D gNormal;
uniform sampler2D gPosition;
uniform sampler2D texNoise;
uniform vec2 ScreenSize;

uniform vec3 noiseKernel[64];
uniform mat4 projection;

float bias = 0.025;

void main()
{
	vec2 noiseScale = ScreenSize / vec2(4.0f);

	vec3 fragPos = texture(gPosition , TexCoords).xyz;
	vec3 normal = normalize(texture(gNormal,TexCoords).rgb);
	vec3 randomVector = normalize(texture(texNoise , TexCoords * noiseScale).xyz);

	vec3 tangent = normalize(randomVector - normal * dot(randomVector,normal));
	vec3 bitangent = cross(normal,tangent);
	mat3 TBN = mat3(tangent,bitangent,normal);

	float occlusion = 0.0f;
	for(int i = 0; i < 64 ; ++i)
	{
       vec3 samplePos = TBN * noiseKernel[i];
	   samplePos = fragPos + samplePos * 0.5;

	   vec4 offset = vec4(samplePos,1.0f);
	   offset = projection * offset;
	   offset.xyz /= offset.w;
	   offset.xyz = offset.xyz * 0.5 + 0.5;

	   float sampleDepth = texture(gPosition,offset.xy).z;

	   float rangeCheck = smoothstep(0.0, 1.0, 0.5 / abs(fragPos.z - sampleDepth));
	   occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / 64);
	FragColor = occlusion;
}