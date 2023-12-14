#ifndef EDGE_DETECT
#define EDGE_DETECT

#include <glew.h>
#include "VectorMath.h"

typedef unsigned int uint;

class FBO
{
public:

	FBO();
	
	void Clear()
	{
		DeleteFBO();
		glDeleteRenderbuffers(1, &rbo);
		glDeleteTextures(1, &screen_image);
	}

	void Bind(GLenum usage);
	void DeleteFBO();
	void DeleteRBO();

	GLuint &GetScreenImage() { return screen_image; };
	GLuint& GetScreenStencilImage() { return screen_stencil_buffer; };
	GLuint &GetFBO() { return fbo; };
	GLuint &GetRBO() { return rbo; };

	Vec2<float> FboSize;

private:

	GLuint fbo;
	GLuint screen_image , screen_stencil_buffer;
	GLuint rbo;

};


   void BindFBONULL();
  
   void CreateCustomFrameBuffer(FBO &fbo, uint width, uint height);




#endif // !EDGE_DETECT


