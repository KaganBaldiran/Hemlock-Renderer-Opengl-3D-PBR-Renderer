#include "SSLS.hpp"

SSLS::SSLS(int width , int height)
{
	CreateCustomFrameBuffer(fbo, width, height);
	SetScreenQuads(VAO, VBO, EBO);
}

void SSLS::Draw(Camera& camera, GLuint SSLSshader, GLuint SSLSbuffer , Vec2<int> GbufferSize , GLuint gbuffer , const glm::vec3 LightPositions[], const float LightIntensities[] , const int &NumberOfLights , const int &MaxLightCount)
{
	glUseProgram(SSLSshader);
	//glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
	fbo.Bind(GL_FRAMEBUFFER);

	glBindVertexArray(VAO);
	glViewport(0, 0, GbufferSize.x, GbufferSize.y);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, SSLSbuffer);
	glUniform1i(glGetUniformLocation(SSLSshader, "SSLSbuffer"), 0);

	GLuint Lpos = glGetUniformLocation(SSLSshader, "lightpositions");
	glUniform3fv(Lpos, MaxLightCount, &LightPositions[0][0]);

	GLuint lightIntensities = glGetUniformLocation(SSLSshader, "lightIntensities");
	glUniform1fv(lightIntensities, MaxLightCount, &LightIntensities[0]);

	glUniform1i(glGetUniformLocation(SSLSshader, "numberoflights"), NumberOfLights);
	glUniformMatrix4fv(glGetUniformLocation(SSLSshader, "modelMat"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(glGetUniformLocation(SSLSshader, "ViewMat"), 1, GL_FALSE, glm::value_ptr(camera.screenratiodefault * camera.projection * camera.cam_view));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void SSLS::Clean()
{
	fbo.Clear();
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void SSLS::SetScreenQuads(GLuint& InputVAO, GLuint& InputVBO, GLuint& InputEBO)
{

	float quadVertices[] = {
		-1.0f,  1.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, 1.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, 0.0f, 0.0f  // bottom-left
	};

	unsigned int quadIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	glGenVertexArrays(1, &InputVAO);
	glGenBuffers(1, &InputVBO);
	glGenBuffers(1, &InputEBO);

	glBindVertexArray(InputVAO);

	glBindBuffer(GL_ARRAY_BUFFER, InputVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, InputEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
