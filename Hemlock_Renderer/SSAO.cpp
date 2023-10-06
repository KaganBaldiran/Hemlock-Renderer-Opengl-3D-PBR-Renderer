#include "SSAO.h"

SSAO::SSAO(Vec2<float> ScreenSize)
{
	this->ScreenSize = ScreenSize;

	std::uniform_real_distribution<float> kernelRandomFloats(0.0f, 1.0f);
	std::default_random_engine generator;

	for (unsigned int i = 0; i < 64; i++)
	{
		glm::vec3 KernelSample(
			kernelRandomFloats(generator) * 2.0f - 1.0f,
			kernelRandomFloats(generator) * 2.0f - 1.0f,
			kernelRandomFloats(generator)
		);
		KernelSample = glm::normalize(KernelSample);
		KernelSample *= kernelRandomFloats(generator);
		float scale = (float)i / 64.0f;
		scale = lerp(0.1f, 1.0f, scale * scale);
		KernelSample *= scale;		
		SSAOkernel.push_back(KernelSample);
	}

	std::vector<glm::vec3> SSAOnoiseTex;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			kernelRandomFloats(generator) * 2.0f - 1.0f,
		    kernelRandomFloats(generator) * 2.0f - 1.0f,
		    0.0f
		);
		SSAOnoiseTex.push_back(noise);
	}
	

	glGenTextures(1, &this->noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &SSAOnoiseTex[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &ssaofbo);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaofbo);
	glGenTextures(1, &ssaoColorAttach);
	glBindTexture(GL_TEXTURE_2D, ssaoColorAttach);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ScreenSize.x, ScreenSize.y, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorAttach, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERR("Cannot complete the frame buffer for SSAO!");
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1 ,&SSAOblurframebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOblurframebuffer);
	glGenTextures(1, &this->SSAOblurColorAttach);
	glBindTexture(GL_TEXTURE_2D, this->SSAOblurColorAttach);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ScreenSize.x, ScreenSize.y, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOblurColorAttach, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERR("Cannot complete the blur frame buffer for SSAO!");
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	float quadVertices[] = { 
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};


	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

SSAO::~SSAO()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteFramebuffers(1, &this->ssaofbo);
	glDeleteFramebuffers(1, &this->SSAOblurframebuffer);
	glDeleteTextures(1, &this->noiseTexture);
	glDeleteTextures(1, &ssaoColorAttach);
	glDeleteTextures(1, &SSAOblurColorAttach);
}

void SSAO::Draw(GLuint SSAOshader , GLuint SSAOblurShader  ,GBUFFER::gBuffer& sceneGbuffer , Camera &camera)
{
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, this->ssaofbo);
	glUseProgram(SSAOshader);
	glViewport(0, 0, ScreenSize.x, ScreenSize.y);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(this->vao);
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneGbuffer.gNormal);
	glUniform1i(glGetUniformLocation(SSAOshader, "gNormal"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sceneGbuffer.gPosition);
	glUniform1i(glGetUniformLocation(SSAOshader, "gPosition"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->noiseTexture);
	glUniform1i(glGetUniformLocation(SSAOshader, "texNoise"), 2);

	glUniform3fv(glGetUniformLocation(SSAOshader, "noiseKernel"), 64, (GLfloat*)&this->SSAOkernel[0]);
	
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)ScreenSize.x / (float)ScreenSize.y, 0.1f, 100.0f);
	glUniform2f(glGetUniformLocation(SSAOshader, "ScreenSize"), ScreenSize.x,ScreenSize.y);
	glUniformMatrix4fv(glGetUniformLocation(SSAOshader, "projection"), 1, GL_FALSE, glm::value_ptr(camera.cammatrix));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	//glBindVertexArray(0);
	//glUseProgram(0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, this->SSAOblurframebuffer);
	glUseProgram(SSAOblurShader);
	//glViewport(0, 0, ScreenSize.x, ScreenSize.y);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//glBindVertexArray(this->vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->ssaoColorAttach);
	glUniform1i(glGetUniformLocation(SSAOblurShader, "SSAOtexture"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_BLEND);
}

float SSAO::lerp(float a, float b, float f)
{
	return a + f * (b - a);
}
