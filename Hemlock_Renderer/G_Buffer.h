#pragma once
#include <glew.h>
#include <glfw3.h>
#include "VectorMath.h"
#include "Log.h"

#define RENDER_PASS_NORMAL 0x022
#define RENDER_PASS_POSITION 0x023
#define RENDER_PASS_ALBEDO 0x024
#define RENDER_PASS_SPECULAR 0x025
#define RENDER_PASS_COMBINED 0x026
#define RENDER_PASS_ID 0x027
#define RENDER_PASS_AO 0x028
#define RENDER_PASS_WIREFRAME 0x029

namespace GBUFFER
{

	class gBuffer
	{
	public:
		gBuffer();
		~gBuffer();

		GLuint gbuffer, gPosition, gNormal, gColorSpec, RBO;
		int window_width, window_height;

	private:

		
	};

	



}

