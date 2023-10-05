#ifndef PICKINGTEXTURE
#define PICKINGTEXTURE

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "VectorMath.h"
#include "Log.h"
typedef unsigned int uint;

struct mouseleftbuttom
{
	bool ispressed = false;
	int x, y;
};

class pickingtexture
{
public:

	pickingtexture(uint w_width, uint w_height);
	~pickingtexture();

	void EnableWriting();
	void DisableWriting();
	

	struct pixelinfo
	{
		uint ObjectID = 0;
		uint DrawID = 0;
		uint PrimID = 0;

		void Print()
		{
			std::cout << "Object " << ObjectID << " draw " << DrawID << " prim " << PrimID << "\n";
		}
	};


	pixelinfo ReadPixel(uint x, uint y, Vec2<int> WindowSize);

	GLuint onMouse(int x, int y , Vec2<uint> windowscale , Vec2<float> menuSize) {
		

		Vec3<float> TranslateCoeff(menuSize.x / windowscale.x, ((windowscale.y - (windowscale.y - 18.0f))) / windowscale.y, 0.0f);
		Vec3<float> ScaleCoeff(((float)windowscale.x - menuSize.x) / windowscale.x, (menuSize.y + 18.0f) / windowscale.y, 1.0f);

		GLbyte color[4];
		GLfloat depth;
		GLuint index;

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		Vec2<float> Ratio((float)mode->width / windowscale.x, (float)mode->height / windowscale.y);

		Vec2<float> mousePos(x, y);
		//mousePos.x /= (float)mode->width;
		//mousePos.y /= (float)mode->height;

		//mousePos = mousePos * Ratio;
		//mousePos.x += menuSize.x;
		//mousePos = mousePos * ScaleCoeff;

		LOG("PIXEL: " << mousePos << " " << mousePos * Ratio);

		x = mousePos.x;
		y = mousePos.y;

		glReadPixels(x, windowscale.y - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);
		glReadPixels(x, windowscale.y - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		glReadPixels(x, windowscale.y - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

	    //std::cout << "Clicked on pixel " << x << " " << y << " " << color[0] << " " << color[1] << " " << color[2] << "" << color[3] << " " << depth << " " << index << "\n";

		printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
			x, y, color[0], color[1], color[2], color[3], depth, index);

		return index;
	}

	GLuint GetPickingTexture() { return this->m_picking_texture; };
	GLuint GetPickingBufferTexture() { return this->picking_buffer_texture; };


private:

	GLuint m_fbo = 0;
	GLuint m_picking_texture = 0;
	GLuint m_depth_texture = 0;
	GLuint picking_buffer_texture;

};


class picking_technique
{
public:

	bool init();

	void DrawStartCB(GLuint shader , uint Drawindex);

	void SetObjectIndex(GLuint shader,uint Objectindex);

};


#endif // !PICKINGTEXTURE
