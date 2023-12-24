#pragma once
#include <glew.h>
#include <glfw3.h>
#include "VectorMath.h"
#include "Log.h"
#include "Camera.h"
#include "post_process.h"

class SSLS
{
public:

	SSLS(int width, int height);
	void Draw(Camera& camera , GLuint SSLSshader, GLuint SSLSbuffer, Vec2<int> GbufferSize, GLuint gbuffer, const glm::vec3 LightPositions[], const float LightIntensities[], const int& NumberOfLights, const int& MaxLightCount);
	void Clean();
	GLuint GetSSLStexture() { return this->fbo.GetScreenImage(); };
	static void SetScreenQuads(GLuint& InputVAO, GLuint& InputVBO, GLuint& InputEBO);
	
private:
	GLuint VBO, VAO, EBO;
	FBO fbo;
};