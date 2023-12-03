#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D Position;  // Screen space position buffer
uniform sampler2D Normal;  // Screen space position buffer

uniform vec3 cameraPosition;
uniform vec3 cameraDirection;
uniform mat4 inverseViewMatrix;
uniform vec2 WindowSize;

uniform float FarNearPlaneDiff;

const int MAX_STEPS = 256;
const float Step = 0.02f;

void main() 
{

    vec4 OutColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    vec2 uv = (gl_FragCoord.xy / WindowSize);
   
    vec3 rayDirection = vec3(uv.x,1.0f - uv.y,1.0f) * normalize(cameraDirection);

    vec3 rayOrigin = cameraPosition;

    vec4 currentScreenPos = inverseViewMatrix * texture(Position, TexCoords);
    currentScreenPos *= 2.0f;
    currentScreenPos -= 1.0f;

    vec4 globalNormal = inverseViewMatrix * texture(Normal, TexCoords);

    float depth = distance(currentScreenPos.xyz , cameraPosition);
    depth /= FarNearPlaneDiff;

    for(int i = 0; i < MAX_STEPS;++i)
    {
        rayOrigin += Step * rayDirection;

        if (depth < distance(rayOrigin , cameraPosition) / FarNearPlaneDiff) // Adjust the threshold as needed
        {
            OutColor = vec4(1.0f); 
            break;
        }
        
    };

    OutColor = vec4(vec3(depth),1.0f);
    FragColor = vec4(pow(OutColor.rgb, vec3(0.9)), OutColor.a);
}
