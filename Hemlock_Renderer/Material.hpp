#pragma once
#include <glew.h>
#include <glfw3.h>
#include "Texture.h"
#include "Shader.h"
#include "VectorMath.h"
#include "Log.h"
#include <map>

#define TEXTURE_DIFFUSE0 "texture_diffuse0"
#define TEXTURE_NORMAL0 "texture_normal0"
#define TEXTURE_SPECULAR0 "texture_specular0"
#define TEXTURE_METALIC0 "texture_metalic0"
#define TEXTURE_ALPHA0 "texture_alpha0"

#define TEXTURE_DIFFUSE1 "texture_diffuse1"
#define TEXTURE_NORMAL1 "texture_normal1"
#define TEXTURE_SPECULAR1 "texture_specular1"
#define TEXTURE_METALIC1 "texture_metalic1"
#define TEXTURE_ALPHA1 "texture_alpha1"


static void SetEnvironment(Shader& shader, float FogIntesity = 1.0f, glm::vec3 FogColor = glm::vec3(1.0f), glm::vec3 EnvironmentRadiance = glm::vec3(1.0f))
{
	shader.setFloat("FogIntesityUniform", FogIntesity);
	shader.setVec3("FogColor", FogColor);
	shader.setVec3("EnvironmentRadiance", EnvironmentRadiance);
}

class Material
{
public:

	Material() : roughness(0.5f), metalic(0.0f), Albedo(1.0f, 1.0f, 1.0f, 1.0f)
	{
		std::fill_n(this->DisableClayMaterial, 4, 1);
	}

	inline void PushTextureMap(const char* Key, Textures& TextureMap)
	{
		TextureMaps[Key] = &TextureMap;

		std::string KeyValue(Key);
		if (KeyValue == TEXTURE_DIFFUSE0)
		{
			DisableClayMaterial[0] = 0;
		}
		else if (KeyValue == TEXTURE_SPECULAR0)
		{
			DisableClayMaterial[1] = 0;
		}
		else if (KeyValue == TEXTURE_NORMAL0)
		{
			DisableClayMaterial[2] = 0;
		}
		else if (KeyValue == TEXTURE_METALIC0)
		{
			DisableClayMaterial[3] = 0;
		}
	}

	inline void PopTextureMap(const char* Key)
	{
		if (TextureMaps.find(Key) != TextureMaps.end())
		{
			std::string KeyValue(Key);
			if (KeyValue == TEXTURE_DIFFUSE0)
			{
				DisableClayMaterial[0] = 1;
			}
			else if (KeyValue == TEXTURE_SPECULAR0)
			{
				DisableClayMaterial[1] = 1;
			}
			else if (KeyValue == TEXTURE_NORMAL0)
			{
				DisableClayMaterial[2] = 1;
			}
			else if (KeyValue == TEXTURE_METALIC0)
			{
				DisableClayMaterial[3] = 1;
			}

			TextureMaps.erase(Key);
		}
	}

	inline Textures* GetTextureMap(const char* Key)
	{
		return TextureMaps[Key];
	}

	inline std::map<std::string, Textures*>& GetTextureMaps()
	{
		return TextureMaps;
	}

	inline void SetMaterialShader(Shader& shader)
	{
		int slot = 0;
		for (auto it = TextureMaps.begin(); it != TextureMaps.end(); ++it)
		{
			it->second->Bind(slot, shader.GetID(), it->first.c_str());
			slot++;
		}

		glUniform1iv(glGetUniformLocation(shader.GetID(), "disableclaymaterial"), 4, &DisableClayMaterial[0]);
		shader.setFloat("metallic", this->metalic);
		shader.setFloat("roughness", this->roughness);
		shader.setVec4("albedo", Albedo);
	}

	inline void EnableClayMaterial()
	{
		std::fill_n(this->DisableClayMaterial, 4, 1);
	}

	float roughness;
	float metalic;
	glm::vec4 Albedo;

private:
	std::map<std::string, Textures*> TextureMaps;
	int DisableClayMaterial[4];
};

