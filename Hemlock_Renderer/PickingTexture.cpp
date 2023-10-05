#include "PickingTexture.h"
#include "Log.h"

pickingtexture::pickingtexture(uint w_width, uint w_height)
{
    
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_picking_texture);
	glBindTexture(GL_TEXTURE_2D, m_picking_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w_width, w_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_picking_texture, 0);

	//glGenTextures(1, &m_depth_texture);
	//glBindTexture(GL_TEXTURE_2D, m_depth_texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R , w_width, w_height, 0, GL_R, GL_FLOAT, NULL);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_depth_texture, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Frame buffer error :: " << status << "\n";
		exit(1);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

pickingtexture::~pickingtexture()
{
	glDeleteFramebuffers(1, &this->m_fbo);
	glDeleteTextures(1, &this->m_picking_texture);
}

void pickingtexture::EnableWriting()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void pickingtexture::DisableWriting()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

pickingtexture::pixelinfo pickingtexture::ReadPixel(uint x, uint y , Vec2<int> WindowSize)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glReadBuffer(GL_COLOR_ATTACHMENT0);

	pixelinfo pixelP;
	float pixel[4];

	glReadPixels(x, WindowSize.y-y, 1,1, GL_RGBA, GL_FLOAT, &pixel);

	LOG("PIXEL PICKED: " << pixel[0] << " " << pixel[1] << " " << pixel[2] << " " << pixel[3] << " " << x << " " << y);

	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pixelP.ObjectID = pixel[0];

	return pixelP;
}

bool picking_technique::init()
{


	return false;
}

void picking_technique::DrawStartCB(GLuint shader, uint Drawindex)
{
	glUniform1ui(glGetUniformLocation(shader, "drawindex"), Drawindex);
}

void picking_technique::SetObjectIndex(GLuint shader,uint Objectindex)
{
	glUniform1ui(glGetUniformLocation(shader, "ObjectID"), Objectindex);
}



