#include "Texture.h"
#include "Shader.h"
#include <glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glfw3.h>
#include <iostream>
#include "Log.h"

Textures::Textures(const char* filepath , GLenum slot , GLenum texturetype , GLenum pixeltype, unsigned int texture_type_for_pbr, std::string texture_type_for_pbr_str)
{
	this->channels = -1;
	std::string temp(filepath);
	path = temp;
	
	this->texture_type_of_pbr = texture_type_for_pbr;
	this->texture_type_of_pbr_str = texture_type_for_pbr_str;
	type = texturetype;
	stbi_set_flip_vertically_on_load(false);
	int channels;
	unsigned char* pixels = stbi_load(filepath, &width, &height, &channels,0);
	if (!pixels)
	{
		TextureState = TEXTURE_ERROR;
		LOG_ERR("Failed to load texture :: " << filepath);
		return;
	}

	this->channels = channels;

	GLenum formattex = NULL;
	if (channels == 1)
	{
		formattex = GL_RED;
	}	
	else if (channels == 3)
	{
		formattex = GL_RGB;

	}
	else if (channels == 4)
	{
		formattex = GL_RGBA;

	}

	LOG_INF("Texture loading :: " << path <<" ::Channel count :: " << channels);

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(texturetype, texture);

	glTexParameteri(texturetype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texturetype, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(texturetype, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texturetype, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(texturetype, 0, formattex, width, height, 0, formattex, pixeltype, pixels);
	glGenerateMipmap(texturetype);

	stbi_image_free(pixels);

	glBindTexture(texturetype, 0);
	TextureState = TEXTURE_SUCCESS;
}

Textures::Textures(const char* filepath, GLenum slot, GLenum texturetype, GLenum pixeltype,GLenum MAG_FILTER , GLenum MIN_FILTER , bool FlipTexture)
{
	this->channels = -1;
	std::string temp(filepath);
	path = temp;

	type = texturetype;
	stbi_set_flip_vertically_on_load(FlipTexture);
	int channels;
	unsigned char* pixels = stbi_load(filepath, &width, &height, &channels, 0);
	if (!pixels)
	{
		TextureState = TEXTURE_ERROR;
		std::cerr << "Failed to load texture :: " << filepath << "\n";
		return;

	}

	this->channels = channels;

	GLenum formattex = NULL;
	if (channels == 1)
	{
		formattex = GL_RED;
	}
	else if (channels == 3)
	{
		formattex = GL_RGB;

	}
	else if (channels == 4)
	{
		formattex = GL_RGBA;

	}

	LOG_INF("Texture loading :: " << path << " ::Channel count :: " << channels);

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(texturetype, texture);

	glTexParameteri(texturetype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texturetype, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(texturetype, GL_TEXTURE_MIN_FILTER, MIN_FILTER);
	glTexParameteri(texturetype, GL_TEXTURE_MAG_FILTER, MAG_FILTER);

	glTexImage2D(texturetype, 0, formattex, width, height, 0, formattex, pixeltype, pixels);
	glGenerateMipmap(texturetype);

	stbi_image_free(pixels);

	glBindTexture(texturetype, 0);
	TextureState = TEXTURE_SUCCESS;
}


Textures::~Textures()
{
}

void Textures::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(type, texture);
}

void Textures::Bind(GLuint slot, GLuint shader, const char* uniform)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glUniform1i(glGetUniformLocation(shader, uniform), slot);
}

void Textures::Unbind()
{
	glBindTexture(type, 0);
	glActiveTexture(GL_TEXTURE0);
}

void Textures::DeleteTexture()
{
	glDeleteTextures(1, &texture);
}

std::string Textures::GetPathData()
{
	return this->path;
}

void Textures::texunit(GLuint shader_program, const char* uniform, GLuint unit)
{
	GLuint texuni = glGetUniformLocation(shader_program, uniform);

	UseShaderProgram(shader_program);

	glUniform1i(texuni, unit);
}

unsigned int Textures::GetTexturePbrType()
{
	return this->texture_type_of_pbr;
}

std::string Textures::GetTexturePbrType_str()
{
	return this->texture_type_of_pbr_str;
}

Vec2<float> GetScreenRatio(GLFWwindow* m_window , Vec2<int> menu_size)
{
	Vec2<int> winsize;
	glfwGetWindowSize(m_window, &winsize.x, &winsize.y);

	winsize.x = menu_size.x;
	winsize.y = menu_size.y;

	float ratio_minmax = NULL;
	float aspectRatios_wh = static_cast<float>(winsize.x) / static_cast<float>(winsize.y);
	float aspectRatios_hw = static_cast<float>(winsize.y) / static_cast<float>(winsize.x);

	float ratio_minmax_x = NULL, ratio_minmax_y = NULL;

	if (winsize.y > winsize.x)
	{
		ratio_minmax_x = aspectRatios_hw;
		ratio_minmax_y = 1.0f;

	}
	if (winsize.y < winsize.x)
	{
		ratio_minmax_x = aspectRatios_hw;
		ratio_minmax_y = 1.0f;

	}
	if (winsize.y == winsize.x)
	{
		ratio_minmax_x = 1.0f;
		ratio_minmax_y = 1.0f;
	}

	return Vec2<float>{ratio_minmax_x, ratio_minmax_y};
}
