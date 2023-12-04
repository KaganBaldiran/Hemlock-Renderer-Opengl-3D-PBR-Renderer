#ifndef CUBEMAP
#define CUBEMAP

#include <glew.h>
#include <glfw3.h>
#include "Buffers.h"
#include <vector>
#include <string>
#include "Shader.h"
#include "Camera.h"

#define HDRI_COMPLETE 0x345
#define HDRI_ERROR 0x346
#define HDRI_INCOMPATIBLE_FILE 0x347

std::pair<GLuint, int> HDRItoCubeMap(const char* HDRI, unsigned int CubeMapSize, GLuint HDRItoCubeMapShader);
std::pair<GLuint, int> ConvolutateCubeMap(GLuint CubeMap, GLuint ConvolutateCubeMapShader);
std::pair<GLuint, int> PreFilterCubeMap(GLuint CubeMap, GLuint PreFilterCubeMapShader);
std::pair<GLuint, int> ComputeLUT(Shader& LUTshader);

class CubeMap
{
public:

	CubeMap(std::vector<std::string> texture_faces, const char* vertex_source, const char* frag_source);
	CubeMap(GLuint CubeMap, const char* vertex_source, const char* frag_source);
	~CubeMap();
	void Draw(Camera& camera , Vec2<float> windowSize);
	GLuint GetCubeMapTexture() { return this->cubemaptextureID; };
	void SetCubeMapTexture(GLuint &CubeMapTexture);


private:

	GLuint cubemaptextureID;
	VBO vbo;
	VAO vao;
	std::vector<std::string> texture_faces;
	Shader* cubemapshader;

};


#endif 

