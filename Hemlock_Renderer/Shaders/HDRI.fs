#version 330 core

out vec4 FragColor;
in vec3 LocalPos;

uniform sampler2D hdriTexture;

const vec2 invAtan =  vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z,v.x) , asin(v.y));
	uv *= invAtan;
	uv += 0.5f;
	return uv;
}
void main()
{
	vec2 uv = SampleSphericalMap(normalize(LocalPos));
	vec3 color = texture(hdriTexture,uv).rgb;
    
	FragColor = vec4(color,1.0f);
}