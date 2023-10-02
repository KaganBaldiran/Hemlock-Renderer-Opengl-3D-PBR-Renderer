#ifndef SHADOWMAP
#define SHADOWMAP

#include <iostream>
#include <glew.h>
#include <glfw3.h>
#include "VectorMath.h"
#include "Buffers.h"
#include "Shader.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <array>



  class shadowmap
  {
  public:

	  shadowmap(uint shadowMapWidth, uint shadowMapHeight)
	  {
		  
		  glGenFramebuffers(1, &shadowMapfbo);

		  this->shadowMapWidth = shadowMapWidth;
		  this->shadowMapHeight = shadowMapHeight;
		  
		  glGenTextures(1, &shadowMap);
		  glBindTexture(GL_TEXTURE_2D, shadowMap);
		  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->shadowMapWidth, this->shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		  float clampcolor[] = { 1.0f,1.0f,1.0f,1.0f };
		  glTextureParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampcolor);

		  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapfbo);
		  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
		  glDrawBuffer(GL_NONE);
		  glReadBuffer(GL_NONE);

		  GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		  if (result == GL_FRAMEBUFFER_COMPLETE)
		  {
			  std::cerr << "Framebuffer is complete." << "\n";

		  }
		  else
		  {
			  std::cerr << "Framebuffer is not complete." << "\n";

		  }

		  // Revert to the default framebuffer for now
		  glBindFramebuffer(GL_FRAMEBUFFER, 0);
		  

	  };

      ~shadowmap()
	  {
		  glDeleteFramebuffers(1, &shadowMapfbo);
		  glDeleteTextures(1, &shadowMap);
	  };


	  void LightProjection(glm::vec3 lightposition,GLuint shader ,GLFWwindow* window,std::vector<Model*> &models ,float scene_scale , Camera camera , Vec2<int> current_viewport_size)
	  {
		 

		  glm::mat4 lightprojection = glm::mat4(1.0f);
		  glm::mat4 orthgonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -20.0f , 20.0f );
		  int height = NULL, width = NULL;
		  glfwGetWindowSize(window, &width, &height);

		  float total_scale = NULL;

		  
		  glm::mat4 CameraPositionTranslation = glm::mat4(1.0f);

		  CameraPositionTranslation = glm::translate(CameraPositionTranslation, glm::vec3(camera.Get_Orientation().x * camera.Get_Position().x, NULL, -camera.Get_Orientation().z * camera.Get_Position().z) / 7.0f);
		  
		  
		  

		  glm::mat4 scale = glm::mat4(1.0f);
		  scale = glm::scale(scale,glm::vec3(200.0f / scene_scale , 200.0f / scene_scale, 200.0f / scene_scale));

		  

		  //glm::mat4 orthgonalProjection = glm::perspective(glm::radians(45.0f), (float)(width/ height), 0.1f, 100.0f);
		  glm::mat4 lightView = glm::lookAt(lightposition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		  //glm::mat4 lightView = glm::lookAt(lightposition, glm::vec3(camera.Get_Position().x,NULL, -camera.Get_Position().z), glm::vec3(0.0f, 1.0f, 0.0f));

		  

		  lightprojection = CameraPositionTranslation * orthgonalProjection * lightView;

		  //lightprojection = lightSpaceMatrix;
		  //translation = glm::translate(translation, -(average_origin_point / 10.0f));


		  lp = lightprojection;

		  UseShaderProgram(shader);

		  glUniformMatrix4fv(glGetUniformLocation(shader, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightprojection));

		  UseShaderProgram(0);

		 

	  };

	  GLuint GetShadowMapFBO() { return this->shadowMapfbo; };

	  uint GetShadowMapImage() { return this->shadowMap; };

	  Vec2<uint> GetShadowMapSize() { return { this->shadowMapWidth, this->shadowMapHeight }; };

	  glm::mat4 GetLightProjection() { return this->lp; };



  private:

	  GLuint shadowMapfbo;
	  GLuint shadowMap;
	  uint shadowMapWidth;
	  glm::mat4 lp;
	  uint shadowMapHeight;
	  
	  

  };





#endif // !1

