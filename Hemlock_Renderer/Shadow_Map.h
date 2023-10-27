#pragma once
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
#include "Camera.h"
#include "Model.h"

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

	  void Draw(std::vector<Model*>& models, GLuint shadow_map_shader, Camera& camera, GLFWwindow* window, glm::vec4 background_colo)
	  {
		  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		  UseShaderProgram(shadow_map_shader);

		  glEnable(GL_DEPTH_TEST);


		  glBindFramebuffer(GL_FRAMEBUFFER,this->shadowMapfbo);

		  glViewport(0, 0, this->shadowMapWidth, this->shadowMapHeight);


		  glClear(GL_DEPTH_BUFFER_BIT);

		  glBindTexture(GL_TEXTURE_2D, shadowMap);


		  for (size_t i = 1; i < models.size(); i++)
		  {
			  models.at(i)->transformation.SendUniformToShader(shadow_map_shader, "model");
			  models[i]->Draw(shadow_map_shader, camera, shadowMap, NULL);
		  }

		  UseShaderProgram(0);

		  glBindFramebuffer(GL_FRAMEBUFFER, 0);

	  }

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

  class OmniShadowMap
  {
  public:
	  OmniShadowMap(float width , float height)
	  {
		  this->ShadowMapSize({ width,height });
		  this->shadowProj = glm::mat4(1.0f);

		  glGenTextures(1, &this->ShadowMapId);
		  glBindTexture(GL_TEXTURE_CUBE_MAP, ShadowMapId);

		  for (size_t i = 0; i < 6; i++)
		  {
			  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			  //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

			  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		  }

		  glGenFramebuffers(1, &depthMapFBO);
		  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		  //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->ShadowMapId, 0);
		  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->ShadowMapId, 0);

		  //glDrawBuffer(GL_NONE);
		  //glReadBuffer(GL_NONE);

		  glBindFramebuffer(GL_FRAMEBUFFER, 0);
		  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	  }

	  void LightMatrix(glm::vec3 lightPos, GLuint shader)
	  {
		  float aspect = ShadowMapSize.x / ShadowMapSize.y;
		  float near = 1.0f;
		 
		  this->shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

		  std::vector<glm::mat4> shadowTransforms;
		  shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		  shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		  shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		  shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		  shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		  shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		  
		  for (size_t i = 0; i < 6; i++)
		  {
			  glUniformMatrix4fv(glGetUniformLocation(shader, ("shadowMatrices[" + std::to_string(i) + "]").c_str()) , 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
		  }
	  }

	  void Draw(GLuint shader , glm::vec3 lightPos_i,std::vector<Model*> &models, Camera& camera)
	  {
		  glUseProgram(shader);
		  glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
		  glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
		  glViewport(0, 0, ShadowMapSize.x, ShadowMapSize.y);
		  glEnable(GL_DEPTH_TEST);
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		  glBindTexture(GL_TEXTURE_CUBE_MAP, this->ShadowMapId);

		  glm::vec3 lightPos = lightPos_i;

		  LightMatrix(lightPos, shader);

		  glUniformMatrix4fv(glGetUniformLocation(shader, "shadowMapProj"), 1, GL_FALSE, glm::value_ptr(this->shadowProj));
		  glUniform3f(glGetUniformLocation(shader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		  glUniform1f(glGetUniformLocation(shader, "farPlane"), far);

		  for (size_t i = 1; i < models.size(); i++)
		  {
			  models.at(i)->transformation.SendUniformToShader(shader, "model");
			  models[i]->Draw(shader, camera, ShadowMapId, NULL);
		  }

		  glBindFramebuffer(GL_FRAMEBUFFER, 0);
		  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		  glUseProgram(0);
	  }

	  GLuint& GetShadowMap() { return this->ShadowMapId; };
	  Vec2<float> GetShadowMapSize() { this->ShadowMapSize; };
	  float GetFarPlane() { return far; };

	  ~OmniShadowMap()
	  {
		  glDeleteTextures(1, &ShadowMapId);
		  glDeleteFramebuffers(1, &depthMapFBO);
	  }

  private:
	  GLuint ShadowMapId , depthMapFBO;
	  Vec2<float> ShadowMapSize;
	  glm::mat4 shadowProj;
	  const float far = 25.0f;
  };





#endif // !1

