#pragma once
#include <glew.h>
#include "Log.h"
#include "VectorMath.h"
#include "G_Buffer.h"
#include "Camera.h"

class SSAO
{
public:
	SSAO(Vec2<float> ScreenSize);
	~SSAO();
	void Draw(GLuint SSAOshader, GLuint SSAOblurShader, GBUFFER::gBuffer& sceneGbuffer, Camera& camera);
	float lerp(float a, float b, float f);
	GLuint GetSSAOfbo() { return this->ssaofbo; };
	GLuint GetSSAOnoiseTexture() { return this->noiseTexture; };
	GLuint GetSSAOTexture() { return this->ssaoColorAttach; };
	GLuint GetSSAOblurredTexture() { return this->SSAOblurColorAttach; };
	std::vector<glm::vec3>& GetNoiseKernel() { return SSAOkernel; };
	Vec2<float> GetTextureSize() { return ScreenSize; };


private:
	GLuint ssaofbo, noiseTexture , ssaoColorAttach , SSAOblurframebuffer , SSAOblurColorAttach;
	GLuint vbo, vao;
	std::vector<glm::vec3> SSAOkernel;
	Vec2<float> ScreenSize;
};




