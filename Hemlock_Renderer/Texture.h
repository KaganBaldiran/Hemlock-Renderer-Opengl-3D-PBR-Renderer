#pragma once
#include "VectorMath.h"
#include <string>


typedef unsigned int GLenum;
typedef unsigned int GLuint;
struct GLFWwindow;

#ifndef TEXTURES
#define TEXTURES 1

#if TEXTURES

#define TEXTURE_ERROR -1
#define TEXTURE_SUCCESS 1

   class Textures
   {
   public:
	   //Creates a copy of the texture
	   Textures(const GLuint& SourceTexture, const GLenum SourceTextureInternalFormat, const glm::vec2 SourceTextureSize, const char* SourceTextureFilePath, GLenum slot, GLenum texturetype, GLenum pixeltype, GLenum MAG_FILTER, GLenum MIN_FILTER);

	   Textures(const char* filepath, GLenum slot, GLenum texturetype, GLenum pixeltype , unsigned int texture_type_for_pbr , std::string texture_type_for_pbr_str);
	   Textures(const char* filepath, GLenum slot, GLenum texturetype, GLenum pixeltype, GLenum MAG_FILTER, GLenum MIN_FILTER, bool FlipTexture);
	   ~Textures();

	   void Bind();
	   void Bind(GLuint slot, GLuint shader, const char* uniform);

	   void Unbind();

	   void DeleteTexture();

	   GLuint* GetTexture() { return &this->texture; };

	   std::string GetPathData();
	   int GetChannelCount() { return this->channels; };

	   int GetTextureWidth() { return this->width; };
	   int GetTextureHeight() { return this->height; };
	   int GetTextureState() { return this->TextureState; };
	   GLenum GetInternalFormat() { return this->InternalFormat; };

	   void texunit(GLuint shader_program, const char* uniform, GLuint unit);

	   unsigned int GetTexturePbrType();
	   std::string GetTexturePbrType_str();



   private:

	   int width;
	   int height;
	   GLuint texture;
	   GLenum type;
	   GLuint unit;
	   GLenum InternalFormat;
	   unsigned int texture_type_of_pbr;
	   std::string texture_type_of_pbr_str;
	   std::string path;
	   int channels;

	   int TextureState;
   };



   Vec2<float> GetScreenRatio(GLFWwindow* m_window, Vec2<int> menu_size);


#endif // INITIALIZE

#endif // !1
