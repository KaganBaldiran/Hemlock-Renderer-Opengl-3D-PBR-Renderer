#pragma once
#ifndef SCENE 
#define SCENE 

#include "Mesh.h"
#include <memory>
#include <FreeImage.h>
#include "Entity.h"
#include "Light.h"
#include "PickingTexture.h"
#include "post_process.h"
#include "SSAO.h"
#include <algorithm>
#include "Shadow_Map.h"
#include "Data.h"
#include "Cubemap.h"
#include "initialize.h"
#include "G_Buffer.h"


#define CURRENT_OBJECT(Current_obj) (Current_obj - 2)

#define X_GIZMO 0x010
#define Y_GIZMO 0x011
#define Z_GIZMO 0x012

class scene
{
public:

	glm::vec4 LightColors[MAX_LIGHT_COUNT];
	glm::vec3 LightPositions[MAX_LIGHT_COUNT];
	int numberoflights = NULL;
	int typeoflights[MAX_LIGHT_COUNT];
	float LightIntensities[MAX_LIGHT_COUNT];
	std::vector<Light*> lights;
	std::vector<Model*> models;
	std::vector<Entity*> Entities;
	float globalscale;


	unsigned int quadVAO, quadVBO;

	scene()
	{
		std::fill_n(LightIntensities, MAX_LIGHT_COUNT, 1.0f);
	}

	int CURRENT_LIGHT(int current_light_index)
	{
		return current_light_index - (2 + models.size()) ;
	}

	void FindGlobalMeshScales()
	{

		float maxX = -std::numeric_limits<float>::infinity();
		float maxY = -std::numeric_limits<float>::infinity();
		float maxZ = -std::numeric_limits<float>::infinity();
		float minX = std::numeric_limits<float>::infinity();
		float minY = std::numeric_limits<float>::infinity();
		float minZ = std::numeric_limits<float>::infinity();

		for (unsigned int i = 0; i < models.size(); i++)
		{

			for (size_t j = 0; j < models[i]->meshes.size(); j++)
			{

				Vertex origin = models[i]->meshes[j].vertices[0]; // use first vertex as origin point

				for (unsigned int k = 0; k < models[i]->meshes[j].vertices.size(); k++) {

					Vertex vertex;
					vertex.Position.x = models[i]->meshes[j].vertices[k].Position.x - origin.Position.x;
					vertex.Position.y = models[i]->meshes[j].vertices[k].Position.y - origin.Position.y;
					vertex.Position.z = models[i]->meshes[j].vertices[k].Position.z - origin.Position.z;// subtract origin point from vertex

					maxX = std::max(maxX, vertex.Position.x);
					maxY = std::max(maxY, vertex.Position.y);
					maxZ = std::max(maxZ, vertex.Position.z);
					minX = std::min(minX, vertex.Position.x);
					minY = std::min(minY, vertex.Position.y);
					minZ = std::min(minZ, vertex.Position.z);

				}
			}


			float meshWidth = maxX - minX;
			float meshHeight = maxY - minY;
			float meshDepth = maxZ - minZ;

			models.at(i)->transformation.ObjectScales.x = meshWidth;
			models.at(i)->transformation.ObjectScales.y = meshHeight;
			models.at(i)->transformation.ObjectScales.z = meshDepth;

			maxX = -std::numeric_limits<float>::infinity();
			maxY = -std::numeric_limits<float>::infinity();
			maxZ = -std::numeric_limits<float>::infinity();
			minX = std::numeric_limits<float>::infinity();
			minY = std::numeric_limits<float>::infinity();
			minZ = std::numeric_limits<float>::infinity();

		}


		float sceneWidth = -std::numeric_limits<float>::infinity();
		float sceneHeight = -std::numeric_limits<float>::infinity();
		float sceneDepth = -std::numeric_limits<float>::infinity();

		for (unsigned int i = 0; i < models.size(); i++)
		{

			sceneWidth = std::max(sceneWidth, models.at(i)->transformation.ObjectScales.x);
			sceneHeight = std::max(sceneHeight, models.at(i)->transformation.ObjectScales.y);;
			sceneDepth = std::max(sceneDepth, models.at(i)->transformation.ObjectScales.z);

		}

		// Decide on a unit scale for the scene
		float unitScale = 1.0f; // 1 unit = 1 meter

		// Calculate the global scale factor
		//float maxDimension = std::max(sceneWidth, std::max(sceneHeight, sceneDepth));
		//float globalScale = maxDimension / unitScale;

		float maxDimension = (sceneWidth + sceneHeight + sceneDepth) / 3;
		float globalScale = maxDimension / unitScale;



		this->globalscale = globalScale;

		for (size_t i = 0; i < models.size(); i++)
		{
			models.at(i)->transformation.scale_avg = (models.at(i)->transformation.ObjectScales.x + models.at(i)->transformation.ObjectScales.y + models.at(i)->transformation.ObjectScales.z) / 3;
			models.at(i)->transformation.dynamic_scale_avg = models.at(i)->transformation.scale_avg;

			models.at(i)->transformation.ObjectScales.x = models.at(i)->transformation.ObjectScales.x ;
			models.at(i)->transformation.ObjectScales.y = models.at(i)->transformation.ObjectScales.y ;
			models.at(i)->transformation.ObjectScales.z = models.at(i)->transformation.ObjectScales.z ;


			std::cout << "Model width: " << models.at(i)->transformation.ObjectScales.x << " Model height: " << models.at(i)->transformation.ObjectScales.y << " Model Depth: " << models.at(i)->transformation.ObjectScales.z << "\n";
			std::cout << "Scale avg: " << models.at(i)->transformation.scale_avg << "\n";

		}

		std::cout << "GLOBAL SCALE: " << globalScale << "\n";


	}

	void FindGlobalLightScales()
	{
		float maxX = -std::numeric_limits<float>::infinity();
		float maxY = -std::numeric_limits<float>::infinity();
		float maxZ = -std::numeric_limits<float>::infinity();
		float minX = std::numeric_limits<float>::infinity();
		float minY = std::numeric_limits<float>::infinity();
		float minZ = std::numeric_limits<float>::infinity();

		for (unsigned int i = 0; i < lights.size(); i++)
		{

			Vertexs origin = lights.at(i)->lightmesh->vertices[0]; // use first vertex as origin point

			for (unsigned int k = 0; k < lights.at(i)->lightmesh->vertices.size(); k++) {

				Vertexs vertex;
				vertex.position.x = lights.at(i)->lightmesh->vertices[k].position.x - origin.position.x;
				vertex.position.y = lights.at(i)->lightmesh->vertices[k].position.y - origin.position.y;
				vertex.position.z = lights.at(i)->lightmesh->vertices[k].position.z - origin.position.z;// subtract origin point from vertex

				maxX = std::max(maxX, vertex.position.x);
				maxY = std::max(maxY, vertex.position.y);
				maxZ = std::max(maxZ, vertex.position.z);
				minX = std::min(minX, vertex.position.x);
				minY = std::min(minY, vertex.position.y);
				minZ = std::min(minZ, vertex.position.z);

			}
			
			float meshWidth = maxX - minX;
			float meshHeight = maxY - minY;
			float meshDepth = maxZ - minZ;

			lights.at(i)->transformation.ObjectScales.x = meshWidth;
			lights.at(i)->transformation.ObjectScales.y = meshHeight;
			lights.at(i)->transformation.ObjectScales.z = meshDepth;

			maxX = -std::numeric_limits<float>::infinity();
			maxY = -std::numeric_limits<float>::infinity();
			maxZ = -std::numeric_limits<float>::infinity();
			minX = std::numeric_limits<float>::infinity();
			minY = std::numeric_limits<float>::infinity();
			minZ = std::numeric_limits<float>::infinity();

		}


		float sceneWidth = -std::numeric_limits<float>::infinity();
		float sceneHeight = -std::numeric_limits<float>::infinity();
		float sceneDepth = -std::numeric_limits<float>::infinity();

		for (unsigned int i = 0; i < lights.size(); i++)
		{

			sceneWidth = std::max(sceneWidth, lights.at(i)->transformation.ObjectScales.x);
			sceneHeight = std::max(sceneHeight, lights.at(i)->transformation.ObjectScales.y);;
			sceneDepth = std::max(sceneDepth, lights.at(i)->transformation.ObjectScales.z);

		}

		// Decide on a unit scale for the scene
		float unitScale = 1.0f; // 1 unit = 1 meter

		float maxDimension = (sceneWidth + sceneHeight + sceneDepth) / 3;
		float globalScale = maxDimension / unitScale;

		//this->globalscale = globalScale;

		for (size_t i = 0; i < lights.size(); i++)
		{
			lights.at(i)->transformation.scale_avg = (lights.at(i)->transformation.ObjectScales.x + lights.at(i)->transformation.ObjectScales.y + lights.at(i)->transformation.ObjectScales.z) / 3;
			lights.at(i)->transformation.dynamic_scale_avg = lights.at(i)->transformation.scale_avg;

			lights.at(i)->transformation.ObjectScales.x = lights.at(i)->transformation.ObjectScales.x;
			lights.at(i)->transformation.ObjectScales.y = lights.at(i)->transformation.ObjectScales.y;
			lights.at(i)->transformation.ObjectScales.z = lights.at(i)->transformation.ObjectScales.z;


			//std::cout << "Light width: " << lights.at(i)->transformation.ObjectScales.x << " Light height: " << lights.at(i)->transformation.ObjectScales.y << " Light Depth: " << lights.at(i)->transformation.ObjectScales.z << "\n";
			//std::cout << "Scale avg: " << lights.at(i)->transformation.scale_avg << "\n";

		}

		//std::cout << "GLOBAL SCALE: " << globalScale << "\n";

	}


	void RecalculateObjectScales(size_t selected_obj , glm::vec3 scale)
	{

		static glm::vec3 prev_scale = glm::vec3(NULL, NULL, NULL);


		if (prev_scale != scale)
		{
			
			//std::cout << "Old global scale: " << globalscale << "\n";

			glm::vec3 scaled;
			scaled = models.at(CURRENT_OBJECT(selected_obj))->transformation.ObjectScales * scale;

			models.at(CURRENT_OBJECT(selected_obj))->transformation.dynamic_scale_avg = (scaled.x  + scaled.y  + scaled.z ) / 3;

			//models.at(CURRENT_OBJECT(selected_obj))->transformation.ObjectScales = scaled;

			globalscale = -std::numeric_limits<float>::infinity();


			for (size_t i = 0; i < models.size(); i++)
			{
				globalscale = std::max(models.at(i)->transformation.dynamic_scale_avg, globalscale);
			}

			//std::cout << "New global scale: " << globalscale << "\n";


			prev_scale = scale;
		}

	}

	void Takescreenshot(int width, int height, const char* path , int renderPass , GBUFFER::gBuffer& gbuffer , FBO& screenFBO)
	{
		
		BYTE* pixels;

		Vec2<int> finalImageSize;
		int ChannelSize = 0;

		if(renderPass == RENDER_PASS_COMBINED || renderPass == RENDER_PASS_WIREFRAME)
		{ 
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			finalImageSize({ (int)width,(int)height });
			ChannelSize = 3;
			pixels = new BYTE[ChannelSize * finalImageSize.x * finalImageSize.y];

			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glReadPixels(0, 0, finalImageSize.x, finalImageSize.y, GL_BGR, GL_UNSIGNED_BYTE, pixels);
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.gbuffer);
			finalImageSize({ gbuffer.window_width , gbuffer.window_height });
			ChannelSize = 4;
			pixels = new BYTE[ChannelSize * finalImageSize.x * finalImageSize.y];
			
			if (renderPass == RENDER_PASS_POSITION)
			{
				glReadBuffer(GL_COLOR_ATTACHMENT0);
				glReadPixels(0, 0, finalImageSize.x, finalImageSize.y, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
			}
			else if (renderPass == RENDER_PASS_NORMAL)
			{
				glReadBuffer(GL_COLOR_ATTACHMENT1);
				glReadPixels(0, 0, finalImageSize.x, finalImageSize.y, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
			}
			else if (renderPass == RENDER_PASS_ALBEDO)
			{
				glReadBuffer(GL_COLOR_ATTACHMENT2);
				glReadPixels(0, 0, finalImageSize.x, finalImageSize.y, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0); 

		}
		
		FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, finalImageSize.x, finalImageSize.y, ChannelSize * finalImageSize.x, 8 * ChannelSize, 0x0000FF, 0xFF0000, 0x00FF00, false);
		FreeImage_Save(FIF_PNG, image, path, 0);

		FreeImage_Unload(image);
		delete[] pixels;

	}

	void Addlight(glm::vec3 meshposition, glm::vec3 meshscale, glm::vec4 light_color, GLuint shader, uint16 light_shape, int lighttype)
	{
		if (!(numberoflights >= MAX_LIGHT_COUNT))
		{
			Light* templight = new Light(meshposition, meshscale, light_color, shader, light_shape, lighttype);
			templight->LightID = numberoflights + 1 + GetModelCount() + 1;
			lights.push_back(templight);
			LightColors[numberoflights] = light_color;
			LightPositions[numberoflights] = meshposition;
			typeoflights[numberoflights] = lighttype;

			FindGlobalLightScales();

			numberoflights++;
		}
		else
		{

			std::cout << "Already reached the sufficent count of lights!" << "\n";

		}


	}

	void ImportModel(std::string filepath, GLuint shader)
	{
		Model* newmodel = new Model(filepath, shader);

		for (size_t i = 0; i < models.size(); i++)
		{
			if (newmodel->ModelName == models.at(i)->ModelName)
			{
				models.at(i)->SameModelInstances++;
				newmodel->ModelName += "(" + std::to_string(models.at(i)->SameModelInstances) + ")";
				break;
			}
		}

		models.push_back(newmodel);

		FindGlobalMeshScales();

	}

	void CopyModel(size_t index)
	{

		Model* newmodel = new Model;

		//newmodel->meshes.assign(->meshes.begin(), models.at(index)->meshes.end());
		for (size_t i = 0; i < models.at(index)->meshes.size(); i++)
		{
			newmodel->meshes.push_back(Mesh(models.at(index)->meshes[i].vertices, models.at(index)->meshes[i].indices, models.at(index)->meshes[i].textures));
			//newmodel->meshes[i].setupMesh();
		}
		//newmodel->textures_loaded.assign(models.at(index)->textures_loaded.begin(), models.at(index)->textures_loaded.end());
		newmodel->directory = models.at(index)->directory;
		newmodel->modelpath = models.at(index)->modelpath;

		std::cout << "copied models Mesh count: " << newmodel->meshes.size() << "\n";
		std::cout << "newmodel->meshes[1].textures.size(): " << newmodel->meshes[0].vertices.size() << "\n";
		std::cout << "models.at(index)->meshes[i].textures: " << models.at(index)->meshes[0].textures.size() << "\n";

		newmodel->transformation = models.at(index)->transformation;
		*newmodel->GetModelIDcounterptr() += 1;
		*GetModel(GetModelCount() - 1)->GetModelIDcounterptr() += 1;
		*newmodel->GetModelIDptr() = GetModelCount() + 2;



		std::cout << "Coppied models ID: " << *newmodel->GetModelIDptr() << "\n";
		//memcpy(newmodel,models.at(index), sizeof(*models.at(index)));

		std::cout << "COPPIED MODEL ID: " << *GetModel(0)->GetModelIDcounterptr() << "\n";
		std::cout << "COPPIED MODEL ID: " << *newmodel->GetModelIDcounterptr() << "\n";

		models.push_back(newmodel);

	}
	/*
	void DrawShadowMap(shadowmap* ShadowMap, GLuint shadow_map_shader, Camera& camera, GLFWwindow* window, glm::vec4 background_color)
	{


		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		UseShaderProgram(shadow_map_shader);

		glEnable(GL_DEPTH_TEST);

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		glBindFramebuffer(GL_FRAMEBUFFER, ShadowMap->GetShadowMapFBO());

		glViewport(0, 0, ShadowMap->GetShadowMapSize().x, ShadowMap->GetShadowMapSize().y);


		glClear(GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, ShadowMap->GetShadowMapImage());

		
		//glCullFace(GL_BACK);

		for (size_t i = 1; i < models.size(); i++)
		{
			models.at(i)->transformation.SendUniformToShader(shadow_map_shader, "model");
			models[i]->Draw(shadow_map_shader, camera, ShadowMap->GetShadowMapImage(),NULL);
		}


		UseShaderProgram(0);
		//glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);



	}
	*/

	void DrawGbuffer(GBUFFER::gBuffer& SceneGbuffer, GLuint GbufferShader, Camera& camera, Vec2<float> menuSize, GLFWwindow& window , int currentselectedobj , std::pair<uint,bool> enablegizmo_p, int currentselectedlight , GLuint pickingtextureShader , pickingtexture& pickingtex , bool Drawlights)
	{
		if (!models.empty())
		{
			UseShaderProgram(GbufferShader);
			glEnable(GL_DEPTH_TEST);

			glBindFramebuffer(GL_FRAMEBUFFER, SceneGbuffer.gbuffer);
			glViewport(0, 0, SceneGbuffer.window_width, SceneGbuffer.window_height);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			for (size_t i = 1; i < models.size(); i++)
			{
				models.at(i)->transformation.SetModelMatrixUniformLocation(GbufferShader, "model");
				models[i]->Draw(GbufferShader, camera, NULL, NULL);
			}

			pickingtex.EnableWriting();
			glViewport(0, 0, SceneGbuffer.window_width, SceneGbuffer.window_height);
			UseShaderProgram(pickingtextureShader);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			for (size_t i = 1; i < models.size(); i++)
			{
				models.at(i)->transformation.SetModelMatrixUniformLocation(pickingtextureShader, "model");
				models[i]->Draw(pickingtextureShader, camera, NULL, NULL);
			}

			if (Drawlights)
			{

				for (int i = 0; i < lights.size(); i++) {
					//glStencilFunc(GL_ALWAYS, i + 1 + GetModelCount() + 1, -1);

					lights[i]->LightID = i + 1 + GetModelCount() + 1;
					lights[i]->Draw(pickingtextureShader, camera);

				}
			}

			if (CURRENT_OBJECT(currentselectedobj) >= NULL || CURRENT_LIGHT(currentselectedlight) >= NULL)
			{
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0xFF);
				glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

				for (int i = 0; i < 3; i++)
				{
					glStencilFunc(GL_ALWAYS, i + 1 + GetModelCount() + 1 + lights.size() + 2, -1);

					DrawGizmo(pickingtextureShader, camera, i, currentselectedobj, enablegizmo_p, currentselectedlight);

				}

				glDepthFunc(GL_LESS);
				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}

			UseShaderProgram(0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	void SetScreenQuads()
	{

		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};


		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	}

	void DrawScreenQuad(GLuint shader, GLuint buffertexture , GBUFFER::gBuffer& screenGbuffer , Vec2<float> menuSize,float viewportHeight , int RenderPass,pickingtexture &pickingTexture,GLuint PickingShader, pickingtexture &pickingBuffertex, SSAO &ssao, bool EnableSSAO,GLFWwindow &window , DATA::UIdataPack& data , Camera& camera)
	{
		int width, height;
		glfwGetWindowSize(&window, &width, &height);

		glViewport(0, 0, width, height);

		glm::vec3 TranslateCoeff(menuSize.x / width, -((height - (height - 18.0f)) / height), 0.0f);
		glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), TranslateCoeff);

		glm::vec3 ScaleCoeff(((float)width - menuSize.x) / width, (menuSize.y + 18.0f) / height, 1.0f);
		glm::mat4 ScaleMat = glm::scale(glm::mat4(1.0f), ScaleCoeff);

		pickingTexture.EnableWriting();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		UseShaderProgram(PickingShader);

		glUniformMatrix4fv(glGetUniformLocation(PickingShader, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat * ScaleMat));

		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pickingBuffertex.GetPickingTexture());
		glUniform1i(glGetUniformLocation(PickingShader, "IDtexture"), 0);
		glUniform1i(glGetUniformLocation(PickingShader, "RenderStep"), 2);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		UseShaderProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
		glClear(GL_COLOR_BUFFER_BIT);

		UseShaderProgram(shader);
		
		if (data.takesreenshot)
		{
			glUniformMatrix4fv(glGetUniformLocation(shader, "modelMat"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		}
		else
		{
		    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat * ScaleMat));
		}
		
		GLuint renderpass = buffertexture;

		if (RenderPass == RENDER_PASS_COMBINED || RenderPass == RENDER_PASS_WIREFRAME)
		{
			renderpass = buffertexture;
			glUniform1i(glGetUniformLocation(shader, "RenderPass"), 1);
			glUniform1i(glGetUniformLocation(shader, "DOFenabled"), (int)data.DOFenabled);
			if (data.DOFenabled)
			{
				glUniform1f(glGetUniformLocation(shader, "DOFfarDistance"), data.DOFfarDistance);
				glUniform1f(glGetUniformLocation(shader, "DOFintensity"), data.DOFintensity);
				glUniform1f(glGetUniformLocation(shader, "FarPlane"), camera.farPlane);
				glUniform1f(glGetUniformLocation(shader, "NearPlane"), camera.nearPlane);
				glUniform3f(glGetUniformLocation(shader, "CameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
				glUniformMatrix4fv(glGetUniformLocation(shader, "ViewMatrix"),1,GL_FALSE,glm::value_ptr(camera.cam_view));

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, screenGbuffer.gPosition);
				glUniform1i(glGetUniformLocation(shader, "PositionBuffer"), 1);
			}
		}
		else if (RenderPass == RENDER_PASS_NORMAL)
		{
			renderpass = screenGbuffer.gNormal;
			glUniform1i(glGetUniformLocation(shader, "RenderPass"), 2);
		}
		else if (RenderPass == RENDER_PASS_ALBEDO)
		{
			renderpass = screenGbuffer.gColorSpec;
			glUniform1i(glGetUniformLocation(shader, "RenderPass"), 3);
		}
		else if (RenderPass == RENDER_PASS_POSITION)
		{
			renderpass = screenGbuffer.gPosition;
			glUniform1i(glGetUniformLocation(shader, "RenderPass"), 4);
		}
		else if (RenderPass == RENDER_PASS_ID)
		{
			renderpass = pickingBuffertex.GetPickingTexture();
			glUniform1i(glGetUniformLocation(shader, "RenderPass"), 5);
		}
		else if (RenderPass == RENDER_PASS_AO)
		{
			renderpass = ssao.GetSSAOblurredTexture();
			glUniform1i(glGetUniformLocation(shader, "RenderPass"), 6);
	    }
		else if (RenderPass == RENDER_PASS_SPECULAR)
		{
			renderpass = screenGbuffer.gColorSpec;
			glUniform1i(glGetUniformLocation(shader, "RenderPass"), 7);
		}
		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderpass);
		glUniform1i(glGetUniformLocation(shader, "Viewport"), 0);

		if (EnableSSAO)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, ssao.GetSSAOblurredTexture());
			glUniform1i(glGetUniformLocation(shader, "SSAO"), 2);
		}

		glUniform1i(glGetUniformLocation(shader, "EnableSSAO"), EnableSSAO);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		UseShaderProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

	}

	void DrawScene(DATA::UIdataPack &data , std::unique_ptr<OmniShadowMap> OmnishadowMaps[] , GLuint OmniShadowShader , 
		Camera &camera,GLFWwindow* window , Shader& PBRShader, Shader& lightshader,FBO& screen_fbo,int RenderPass , int &currentselectedobj,
		Shader& Outlineshader , CubeMap& Cubemap , int &currentselectedlight , std::pair<uint , bool> &enablegizmo_p,SSAO& ssao , Shader & SSAOShader,
		Shader& SSAOblurShader , GBUFFER::gBuffer& SceneGbuffer ,int width , int height , Meshs& grid , Vec2<int> &current_viewport_size)
	{
		if (data.RenderShadows)
		{
			for (size_t i = 0; i < glm::min(data.ShadowCastingLightCount, numberoflights); i++)
			{
				OmnishadowMaps[i]->Draw(OmniShadowShader, LightPositions[i], models, camera);
			}
		}
		WindowSizeRecall(window, current_viewport_size);


		screen_fbo.Bind(GL_FRAMEBUFFER);
		if (data.takesreenshot)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glViewport(0, 0, mode->width, mode->height);
		}
		else
		{
		   glViewport(0, 0, screen_fbo.FboSize.x, screen_fbo.FboSize.y);
		}

		if (RenderPass == RENDER_PASS_COMBINED || RenderPass == RENDER_PASS_WIREFRAME)
		{

			glClearColor(data.clear_color.x, data.clear_color.y, data.clear_color.z, data.clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			glEnable(GL_STENCIL_TEST);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


			//glEnable(GL_FRAMEBUFFER_SRGB);

			if (currentselectedobj >= 2)
			{
				DrawModelsWithOutline(PBRShader.GetID(), Outlineshader.GetID(), camera, currentselectedobj - 2, currentselectedobj, NULL);

			}

			if (data.RenderGrid)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				RenderGrid(lightshader.GetID(), grid, camera);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			if (data.takesreenshot)
			{
				glClearColor(data.clear_color.x, data.clear_color.y, data.clear_color.z, data.clear_color.w);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				//camera.screenratiodefault = glm::mat4(1.0f);
			}

			if (data.render_cube_map)
			{
				Cubemap.Draw(camera, { (float)width,(float)height });
			}

			UseShaderProgram(PBRShader.GetID());

			glUniform3f(glGetUniformLocation(PBRShader.GetID(), "albedo"), data.albedo.x, data.albedo.y, data.albedo.z);
			glUniform1f(glGetUniformLocation(PBRShader.GetID(), "metallic"), data.metallic);
			glUniform1f(glGetUniformLocation(PBRShader.GetID(), "roughness"), data.roughness);
			glUniform1f(glGetUniformLocation(PBRShader.GetID(), "ao"), data.ao);

			for (int i = 0; i < GetModelCount() + 1; i++) {
				glStencilFunc(GL_ALWAYS, i + 1, -1);
				if (i == 0)
				{


				}
				if (i > 1)
				{

					//ShadowMap.LightProjection(scene.LightPositions[0], PBRShader.GetID(), window, scene.models, scene.globalscale, camera, UI::current_viewport_size);
					if (RenderPass == RENDER_PASS_COMBINED || RenderPass == RENDER_PASS_WIREFRAME)
					{
						UseShaderProgram(PBRShader.GetID());

						glUniform1i(glGetUniformLocation(PBRShader.GetID(), "enablehighlight"), data.enablehighlight);

						GetModel(i - 1)->transformation.SetModelMatrixUniformLocation(PBRShader.GetID(), "model");

						auto ShaderPrep = [&]() {

							glUniform1i(glGetUniformLocation(PBRShader.GetID(), "RenderShadows"), data.RenderShadows);

							glActiveTexture(GL_TEXTURE0 + 4);
							glBindTexture(GL_TEXTURE_CUBE_MAP, data.ConvDiffCubeMap);
							glUniform1i(glGetUniformLocation(PBRShader.GetID(), "ConvCubeMap"), 4);

							glActiveTexture(GL_TEXTURE0 + 5);
							glBindTexture(GL_TEXTURE_CUBE_MAP, data.PrefilteredEnvMap);
							glUniform1i(glGetUniformLocation(PBRShader.GetID(), "prefilteredMap"), 5);

							glActiveTexture(GL_TEXTURE0 + 6);
							glBindTexture(GL_TEXTURE_2D, data.brdfLUT);
							glUniform1i(glGetUniformLocation(PBRShader.GetID(), "brdfLUT"), 6);

							glUniform1f(glGetUniformLocation(PBRShader.GetID(), "FarPlane"), camera.farPlane);
							glUniform1f(glGetUniformLocation(PBRShader.GetID(), "NearPlane"), camera.nearPlane);
							glUniform1i(glGetUniformLocation(PBRShader.GetID(), "FogEnabled"), (int)data.FogEnabled);
							glUniform1f(glGetUniformLocation(PBRShader.GetID(), "FogIntensityUniform"), data.FogIntensity);
							glUniform3f(glGetUniformLocation(PBRShader.GetID(), "FogColor"), data.FogColor.x, data.FogColor.y, data.FogColor.z);


							if (data.RenderShadows)
							{
								glUniform1f(glGetUniformLocation(PBRShader.GetID(), "farPlane"), 25.0f);
								glUniform1i(glGetUniformLocation(PBRShader.GetID(), "ShadowCastingLightCount"), data.ShadowCastingLightCount);

								for (size_t i = 0; i < glm::min(data.ShadowCastingLightCount, numberoflights); i++)
								{
									glActiveTexture(GL_TEXTURE0 + 7 + i);
									glBindTexture(GL_TEXTURE_CUBE_MAP, OmnishadowMaps[i]->GetShadowMap());
									glUniform1i(glGetUniformLocation(PBRShader.GetID(), ("OmniShadowMaps[" + std::to_string(i) + "]").c_str()), 7 + i);
								}
							}

							};

						DrawModelsMultipleShadowMaps(PBRShader.GetID(), camera, i - 1, ShaderPrep, NULL);

						glActiveTexture(GL_TEXTURE0);

						if (RenderPass == RENDER_PASS_WIREFRAME)
						{
							UseShaderProgram(lightshader.GetID());

							glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
							glEnable(GL_CULL_FACE);
							glCullFace(GL_BACK);
							glUniform4f(glGetUniformLocation(lightshader.GetID(), "lightColor"), 1.0f, 1.0f, 1.0f, 1.0f);

							GetModel(i - 1)->transformation.SetModelMatrixUniformLocation(lightshader.GetID(), "model");
							DrawModels(lightshader.GetID(), camera, i - 1, NULL, NULL);

							UseShaderProgram(0);
							glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
							glDisable(GL_CULL_FACE);
						}
					}
					

				}


			}
			UseShaderProgram(0);


			if (data.renderlights)
			{

				for (int i = 0; i < lights.size(); i++) {
					glStencilFunc(GL_ALWAYS, i + 1 + GetModelCount() + 1, -1);

					lights[i]->Draw(lightshader.GetID(), camera);

				}
			}
			if (CURRENT_OBJECT(currentselectedobj) >= NULL || CURRENT_LIGHT(currentselectedlight) >= NULL)
			{
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0xFF);
				glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

				for (int i = 0; i < 3; i++)
				{
					glStencilFunc(GL_ALWAYS, i + 1 + GetModelCount() + 1 + lights.size() + 2, -1);

					DrawGizmo(lightshader.GetID(), camera, i, currentselectedobj, enablegizmo_p, currentselectedlight);

				}

				glDepthFunc(GL_LESS);
				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}

			//glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}


		glBindFramebuffer(GL_FRAMEBUFFER, NULL);

		/*if (data.takesreenshot)
		{
			UseShaderProgram(0);

			Vec2<int> screensize;
			glfwGetWindowSize(window, &screensize.x, &screensize.y);

			Takescreenshot(screensize.x, screensize.y, data.screenshotPathstr.c_str(), RenderPass, SceneGbuffer, screen_fbo);
			data.takesreenshot = false;
		}*/

		if (data.EnableSSAO)
		{
			ssao.Draw(SSAOShader.GetID(), SSAOblurShader.GetID(), SceneGbuffer, camera,data);
		}
	}


	void DrawSSGUScreenQuad(GLuint shader, GLuint buffertexture, GBUFFER::gBuffer& screenGbuffer, Vec2<float> menuSize, float viewportHeight, int RenderPass, pickingtexture& pickingTexture, GLuint PickingShader, pickingtexture& pickingBuffertex, SSAO& ssao, bool EnableSSAO, GLFWwindow& window , Camera &camera)
	{
		int width, height;
		glfwGetWindowSize(&window, &width, &height);

		glViewport(0, 0, width, height);

		glm::vec3 TranslateCoeff(menuSize.x / width, -((height - (height - 18.0f)) / height), 0.0f);
		glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), TranslateCoeff);

		glm::vec3 ScaleCoeff(((float)width - menuSize.x) / width, (menuSize.y + 18.0f) / height, 1.0f);
		glm::mat4 ScaleMat = glm::scale(glm::mat4(1.0f), ScaleCoeff);

		pickingTexture.EnableWriting();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		UseShaderProgram(PickingShader);

		glUniformMatrix4fv(glGetUniformLocation(PickingShader, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat * ScaleMat));

		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pickingBuffertex.GetPickingTexture());
		glUniform1i(glGetUniformLocation(PickingShader, "IDtexture"), 0);
		glUniform1i(glGetUniformLocation(PickingShader, "RenderStep"), 2);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		UseShaderProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		UseShaderProgram(shader);
		
	    glUniform3f(glGetUniformLocation(shader, "cameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform3f(glGetUniformLocation(shader, "cameraDirection"), camera.Orientation.x, camera.Orientation.y, camera.Orientation.z);
		glUniform2f(glGetUniformLocation(shader, "WindowSize"), width , height);

		glUniform1f(glGetUniformLocation(shader, "FarNearPlaneDiff"), camera.farPlane - camera.nearPlane);

		glUniformMatrix4fv(glGetUniformLocation(shader, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat * ScaleMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "inverseViewMatrix"), 1, GL_FALSE, glm::value_ptr(glm::inverse(camera.cam_view)));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, buffertexture);
		glUniform1i(glGetUniformLocation(shader, "Viewport"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, screenGbuffer.gPosition);
		glUniform1i(glGetUniformLocation(shader, "Position"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, screenGbuffer.gNormal);
		glUniform1i(glGetUniformLocation(shader, "Normal"), 2);

		if (EnableSSAO)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, ssao.GetSSAOblurredTexture());
			glUniform1i(glGetUniformLocation(shader, "SSAO"), 3);
		}

		glUniform1i(glGetUniformLocation(shader, "EnableSSAO"), EnableSSAO);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		UseShaderProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

	}


	void DrawModelsWithOutline(GLuint shader1, GLuint shader2, Camera& camera, size_t model_index_to_draw, size_t current_selected, GLuint shadowMap)
	{
		//static bool AlreadySelected = false;
		glClear(GL_STENCIL_BUFFER_BIT);

		UseShaderProgram(shader1);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glDisable(GL_DEPTH_TEST);

		models[model_index_to_draw]->transformation.SetModelMatrixUniformLocation(shader1, "model");
		models[model_index_to_draw]->Draw(shader1, camera, shadowMap,NULL);

		//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glEnable(GL_DEPTH_TEST);

		//glDepthFunc(GL_NEVER);
		UseShaderProgram(shader2);

		//glm::mat4 temp = *models[model_index_to_draw]->transformation.GetTransformationMat4();
		//SetGrid(shader2).vertices[0] /
		//models[model_index_to_draw]->transformation.translate(glm::vec3(0.0f, -0.5f, 0.0f));

		//models[model_index_to_draw]->transformation.scale(glm::vec3(1.01f, 1.01f, 1.01f));
		models[model_index_to_draw]->transformation.SetModelMatrixUniformLocation(shader2, "model");
		glUniformMatrix4fv(glGetUniformLocation(shader2, "ViewMat"), 1, GL_FALSE, glm::value_ptr(camera.cam_view));

		//std::cout << "SCALE AVG@; " << models[model_index_to_draw]->transformation.scale_avg << "\n";

		UseShaderProgram(shader2);

		glUniform1f(glGetUniformLocation(shader2, "scale_avg"), models[model_index_to_draw]->transformation.scale_avg);

		//models[model_index_to_draw]->transformation.translate(-glm::vec3(0.0f, -0.5f, 0.0f));

		models[model_index_to_draw]->Draw(shader2, camera, shadowMap,NULL);

		UseShaderProgram(NULL);

		//*models[model_index_to_draw]->transformation.GetTransformationMat4() = temp;
		//models[model_index_to_draw]->transformation.SendUniformToShader(shader2, "model");

		glDepthFunc(GL_LESS);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);

	}
	void DrawModels(GLuint shader, Camera& camera, size_t model_index_to_draw, GLuint shadowMap , GLuint cube_map_texture)
	{

		models[model_index_to_draw]->Draw(shader, camera, shadowMap,cube_map_texture);

	}

	void DrawModelsMultipleShadowMaps(GLuint shader, Camera& camera, size_t model_index_to_draw,  std::function<void()> ShaderPreperation, GLuint cube_map_texture)
	{

		models[model_index_to_draw]->DrawMultipleShadowMaps(shader, camera, ShaderPreperation, cube_map_texture);

	}

	void DrawModels(GLuint shader, Camera& camera, size_t model_index_to_draw, GLuint shadowMap, GLuint cube_map_texture , GLuint SSAO_tex)
	{

		models[model_index_to_draw]->Draw(shader, camera, shadowMap, cube_map_texture , SSAO_tex);

	}

	Model* GetModel(size_t model_index)
	{
		return models[model_index];
	}

	size_t GetModelCount()
	{
		return models.size();
	}

	void DeleteModel(size_t index)
	{
		delete models.at(index);
		models.erase(models.begin() + index);
	}


	void DeleteLight(size_t index , GLuint shader)
	{
		if (lights.size() > 1)
		{
			for (size_t i = index; i < numberoflights - 1; i++)
			{
				LightColors[i] = LightColors[i + 1];
				LightIntensities[i] = LightIntensities[i + 1];
				LightPositions[i] = LightPositions[i + 1];

				lights.at(i)->LightID = lights.at(i + 1)->LightID;
			}
		}
		
		delete lights.at(index);
		lights.erase(lights.begin() + index);
		numberoflights--;

		handlelights(shader);
	}

	~scene()
	{
		for (auto temp : lights)
		{
			delete temp;
		}

		for (auto temp : models)
		{
			delete temp;
		}
	}

	void handlelights(GLuint shader)
	{
		UseShaderProgram(shader);

		GLuint Lcolors = glGetUniformLocation(shader, "lightColors");
		glUniform4fv(Lcolors, MAX_LIGHT_COUNT, &LightColors[0][0]);

		GLuint Lpos = glGetUniformLocation(shader, "lightpositions");
		glUniform3fv(Lpos, MAX_LIGHT_COUNT, &LightPositions[0][0]);

		GLuint numberofl = glGetUniformLocation(shader, "numberoflights");
		glUniform1i(numberofl, numberoflights);

		GLuint typeoflight = glGetUniformLocation(shader, "typeoflight");
		glUniform1iv(typeoflight, MAX_LIGHT_COUNT, typeoflights);

		GLuint lightIntensities = glGetUniformLocation(shader, "lightIntensities");
		glUniform1fv(lightIntensities, MAX_LIGHT_COUNT, &LightIntensities[0]);

		UseShaderProgram(0);

	}

	void DrawLights(GLuint shader, Camera& camera)
	{

		for (size_t i = 0; i < numberoflights; i++)
		{

			lights[i]->Draw(shader, camera);

		}

	}

	std::pair<std::vector<Vertexs>, std::vector<uint>> Grid(size_t grid_incr)
	{

		std::vector<Vertexs> vertices;
		std::vector<uint> indices;

		for (int j = 0; j <= grid_incr; ++j) {
			for (int i = 0; i <= grid_incr; ++i) {
				float x = (float)i / (float)grid_incr;
				float y = 0;
				float z = (float)j / (float)grid_incr;
				Vertexs temp = { glm::vec3(x, y, z) };
				vertices.push_back(temp);
			}
		}

		for (int j = 0; j < grid_incr; ++j) {
			for (int i = 0; i < grid_incr; ++i) {

				int row1 = j * (grid_incr + 1);
				int row2 = (j + 1) * (grid_incr + 1);

				indices.push_back(row1 + i);
				indices.push_back(row1 + i + 1);
				indices.push_back(row1 + i + 1);
				indices.push_back(row2 + i + 1);
				indices.push_back(row2 + i + 1);
				indices.push_back(row2 + i);
				indices.push_back(row2 + i);
				indices.push_back(row1 + i);

			}
		}


		return { vertices,indices };

	}

	Meshs SetGrid(GLuint shader)
	{

		std::vector<Vertexs> gridvert = Grid(10).first;
		std::vector<uint> gridindices = Grid(10).second;
		std::vector<Textures> tex;

		Meshs grid(gridvert, gridindices, tex, shader);

		return grid;

	}

	void RenderGrid(GLuint shader, Meshs& grid, Camera& camera)
	{
		UseShaderProgram(shader);

		glm::mat4 modelmat = glm::mat4(1.0f);

		modelmat = glm::translate(modelmat, glm::vec3(-0.5f, 0.0f, -0.5f));
		modelmat = glm::scale(modelmat, glm::vec3(1.0f, 1.0f, 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(modelmat));
		glUniform4f(glGetUniformLocation(shader, "lightColor"), 1.0f,1.0f,1.0f,1.0f);

		camera.Matrix(shader, "cameramatrix");
		grid.Draw(shader, camera, GL_LINES);

		UseShaderProgram(0);

	}

	void DrawGizmo(GLuint shader, Camera& camera , size_t iterator , int currentselectedobject , std::pair<uint , bool> enablegizmo_p , int currentselectedlight)
	{

		int Model_index = -1;
		//glm::vec3 model_transformation;
		glm::mat4 model_transformation;
		glm::vec3 model_scales;
		glm::vec3 originpoint;

		if (CURRENT_LIGHT(currentselectedlight) >= NULL)
		{
			Model_index = CURRENT_LIGHT(currentselectedlight);

			model_transformation = lights.at(Model_index)->transformation.TranslationMatrix;

			model_scales = glm::vec3(lights.at(Model_index)->transformation.ScalingMatrix[0][0],
				                     lights.at(Model_index)->transformation.ScalingMatrix[1][1],
				                     lights.at(Model_index)->transformation.ScalingMatrix[2][2]);

			originpoint = glm::vec3(lights.at(Model_index)->originpoint.x,
				                    lights.at(Model_index)->originpoint.y,
				                    lights.at(Model_index)->originpoint.z);

		}
		else if(CURRENT_OBJECT(currentselectedobject) >= NULL)
		{
			Model_index = CURRENT_OBJECT(currentselectedobject);

			model_transformation = models.at(Model_index)->transformation.TranslationMatrix;


			model_scales = glm::vec3(models.at(Model_index)->transformation.ScalingMatrix[0][0],
				                     models.at(Model_index)->transformation.ScalingMatrix[1][1],
				                     models.at(Model_index)->transformation.ScalingMatrix[2][2]);

			originpoint = glm::vec3(GetModel(Model_index)->transformation.Position.x ,
				                    GetModel(Model_index)->transformation.Position.y,
				                    GetModel(Model_index)->transformation.Position.z);

		}

		GetModel(0)->transformation.Translate(originpoint);
		GetModel(0)->transformation.TranslationMatrix = model_transformation;

		GetModel(0)->transformation.Scale(model_scales * 10.0f);

		if (CURRENT_LIGHT(currentselectedlight) >= NULL)
		{

			GetModel(0)->transformation.Scale(glm::vec3(lights.at(Model_index)->transformation.scale_avg) / 5.0f);
			//GetModel(0)->transformation.scale(glm::vec3(GetModel(0)->transformation.scale_avg) / 5.0f);


		}
		else if (CURRENT_OBJECT(currentselectedobject) >= NULL)
		{
			GetModel(0)->transformation.Scale(glm::vec3(GetModel(Model_index)->transformation.scale_avg) / 5.0f);

		}
		UseShaderProgram(shader);


		if (iterator == 0)
		{
			GetModel(0)->transformation.SetModelMatrixUniformLocation(shader, "model");
			UseShaderProgram(shader);

			if (enablegizmo_p.first == Y_GIZMO && enablegizmo_p.second)
			{
				glUniform4f(glGetUniformLocation(shader, "lightColor"), 0.0f, 1.0f, 0.0f, 1.0f);
			}
			else
			{
				glUniform4f(glGetUniformLocation(shader, "lightColor"), 0.0f, 0.7f, 0.0f, 1.0f);
			}
			*GetModel(0)->GetModelIDptr() = 4 + GetModelCount() + lights.size();

			UseShaderProgram(0);
			DrawModels(shader, camera, 0, NULL,NULL);

		}
		if (iterator == 1)
		{
			GetModel(0)->transformation.Rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			GetModel(0)->transformation.SetModelMatrixUniformLocation(shader, "model");
			UseShaderProgram(shader);

			if (enablegizmo_p.first == Z_GIZMO && enablegizmo_p.second)
			{
				glUniform4f(glGetUniformLocation(shader, "lightColor"), 1.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				glUniform4f(glGetUniformLocation(shader, "lightColor"), 0.7f, 0.0f, 0.0f, 1.0f);
			}

			*GetModel(0)->GetModelIDptr() = 5 + GetModelCount() + lights.size();
			UseShaderProgram(0);
			DrawModels(shader, camera, 0, NULL,NULL);
			GetModel(0)->transformation.Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		if (iterator == 2)
		{
			GetModel(0)->transformation.Rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			GetModel(0)->transformation.Rotate(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
			GetModel(0)->transformation.SetModelMatrixUniformLocation(shader, "model");
			UseShaderProgram(shader);

			if (enablegizmo_p.first == X_GIZMO && enablegizmo_p.second)
			{
				glUniform4f(glGetUniformLocation(shader, "lightColor"), 0.0f, 0.0f, 1.0f, 1.0f);
			}
			else
			{
				glUniform4f(glGetUniformLocation(shader, "lightColor"), 0.0f, 0.0f, 0.7f, 1.0f);
			}

			*GetModel(0)->GetModelIDptr() = 6 + GetModelCount() + lights.size();

			UseShaderProgram(0);
			DrawModels(shader, camera, 0, NULL,NULL);
			GetModel(0)->transformation.Rotate(-90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
			GetModel(0)->transformation.Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));


		}

		if (CURRENT_LIGHT(currentselectedlight) >= NULL)
		{

		   GetModel(0)->transformation.Scale(1.0f / (glm::vec3(lights.at(Model_index)->transformation.scale_avg / 5.0f)));
		   //GetModel(0)->transformation.scale(1.0f / (glm::vec3(GetModel(0)->transformation.scale_avg / 5.0f)));

		}
		else if (CURRENT_OBJECT(currentselectedobject) >= NULL)
		{

			GetModel(0)->transformation.Scale(1.0f / (glm::vec3(GetModel(Model_index)->transformation.scale_avg / 5.0f)));
		}

		GetModel(0)->transformation.Scale(1.0f / (model_scales * 10.0f));

		GetModel(0)->transformation.Translate(-originpoint);
		
		*GetModel(0)->GetModelIDptr() = 2;

		

	}

	Vec2<double> UseGizmo(GLFWwindow* window , int &currentselectedgizmo , int currentselectedobj, std::pair<uint , bool> &enablegizmo_p , Vec2<double> PrevMousePos , Camera camera , int currentselectedlight , GLuint Model_Shader , GLuint PBR_Shader, Vec2<double> &temp_mouse)
	{

		//LOG("DOT PRODUCT: " << Vec4<float>(camera.cam_view * glm::vec4(0.0f, 0.0f, 1.0f,0.0f)));


		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			enablegizmo_p = { NULL,false };
			currentselectedgizmo = NULL;
		}

		if (currentselectedgizmo == (4 + GetModelCount() + lights.size()) && !enablegizmo_p.second)
		{
			enablegizmo_p = { Y_GIZMO, true };
		}
		else if (currentselectedgizmo == (5 + GetModelCount() + lights.size()) && !enablegizmo_p.second)
		{
			enablegizmo_p = { Z_GIZMO, true };
		}
		else if (currentselectedgizmo == (6 + GetModelCount() + lights.size()) && !enablegizmo_p.second)
		{
			enablegizmo_p = { X_GIZMO, true };
		}

		Vec2<double> delta_mouse = { temp_mouse.x - PrevMousePos.x, temp_mouse.y - PrevMousePos.y };

		if (CURRENT_OBJECT(currentselectedobj) >= NULL)
		{
			if (enablegizmo_p.first == Y_GIZMO && enablegizmo_p.second == true)
			{
				GetModel(CURRENT_OBJECT(currentselectedobj))->transformation.Translate(glm::vec3(NULL, -delta_mouse.y / 20.0f, NULL));
				GetModel(CURRENT_OBJECT(currentselectedobj))->dynamic_origin += glm::vec3(NULL, -delta_mouse.y / 20.0f, NULL);
			}
			else if (enablegizmo_p.first == Z_GIZMO && enablegizmo_p.second == true)
			{

				double active_delta_mouse = NULL;
				glm::vec4 ViewSpaceZaxis = camera.cam_view * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
				float absX = glm::abs(ViewSpaceZaxis.x);
				float absY = glm::abs(ViewSpaceZaxis.y);
				float absZ = glm::abs(ViewSpaceZaxis.z);

				float max = glm::max(glm::max(absX, absY), absZ);

				if (max == absX)
				{
					active_delta_mouse = delta_mouse.x;

					if (ViewSpaceZaxis.x < 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
					else if (ViewSpaceZaxis.x > 0)
					{
					    active_delta_mouse = active_delta_mouse;
					}
				}
				else if (max == absY)
				{
					active_delta_mouse = delta_mouse.y;

					if (ViewSpaceZaxis.y < 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
					else if (ViewSpaceZaxis.y > 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
				}
				else if (max == absZ)
				{
					active_delta_mouse = delta_mouse.y;

					if (ViewSpaceZaxis.z < 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
					else if (ViewSpaceZaxis.z > 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
				}
	
				GetModel(CURRENT_OBJECT(currentselectedobj))->transformation.Translate(glm::vec3(NULL, NULL, active_delta_mouse / 20.0f));
				GetModel(CURRENT_OBJECT(currentselectedobj))->dynamic_origin += glm::vec3(NULL, NULL, active_delta_mouse / 20.0f);
			}

			else if (enablegizmo_p.first == X_GIZMO && enablegizmo_p.second == true)
			{

				double active_delta_mouse = NULL;

				glm::vec4 ViewSpaceXaxis = camera.cam_view * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
				float absX = glm::abs(ViewSpaceXaxis.x);
				float absY = glm::abs(ViewSpaceXaxis.y);
				float absZ = glm::abs(ViewSpaceXaxis.z);

				float max = glm::max(glm::max(absX, absY), absZ);

				if (max == absX)
				{
					active_delta_mouse = delta_mouse.x;

					if (ViewSpaceXaxis.x < 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
					else if (ViewSpaceXaxis.x > 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
				}
				else if (max == absY)
				{
					active_delta_mouse = delta_mouse.y;

					if (ViewSpaceXaxis.y < 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
					else if (ViewSpaceXaxis.y > 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
				}
				else if (max == absZ)
				{
					active_delta_mouse = delta_mouse.y;

					if (ViewSpaceXaxis.z < 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
					else if (ViewSpaceXaxis.z > 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
				}

				GetModel(CURRENT_OBJECT(currentselectedobj))->transformation.Translate(glm::vec3(active_delta_mouse / 20.0f, NULL, NULL));
				GetModel(CURRENT_OBJECT(currentselectedobj))->dynamic_origin += glm::vec3(active_delta_mouse / 20.0f, NULL, NULL);
			}
		}

		if (CURRENT_LIGHT(currentselectedlight) >= NULL)
		{

			Light* currentlight = lights.at(CURRENT_LIGHT(currentselectedlight));
			const float lightSpeed = 0.004f;

			if (enablegizmo_p.first == Y_GIZMO && enablegizmo_p.second == true)
			{

				float deltaTransformation = -delta_mouse.y * lightSpeed;
				currentlight->transformation.Translate(glm::vec3(NULL, deltaTransformation, NULL));

				currentlight->lightpos += glm::vec3(NULL, deltaTransformation, NULL);

				currentlight->originpoint += glm::vec3(NULL, deltaTransformation, NULL);

				LightPositions[CURRENT_LIGHT(currentselectedlight)] = currentlight->lightpos;

				currentlight->lightmodel = currentlight->transformation.GetModelMat4();

				handlelights(PBR_Shader);


			}
			else if (enablegizmo_p.first == Z_GIZMO && enablegizmo_p.second == true)
			{

				double active_delta_mouse = NULL;
				glm::vec4 ViewSpaceZaxis = camera.cam_view * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
				float absX = glm::abs(ViewSpaceZaxis.x);
				float absY = glm::abs(ViewSpaceZaxis.y);
				float absZ = glm::abs(ViewSpaceZaxis.z);

				float max = glm::max(glm::max(absX, absY), absZ);

				if (max == absX)
				{
					active_delta_mouse = delta_mouse.x;

					if (ViewSpaceZaxis.x < 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
					else if (ViewSpaceZaxis.x > 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
				}
				else if (max == absY)
				{
					active_delta_mouse = delta_mouse.y;

					if (ViewSpaceZaxis.y < 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
					else if (ViewSpaceZaxis.y > 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
				}
				else if (max == absZ)
				{
					active_delta_mouse = delta_mouse.y;

					if (ViewSpaceZaxis.z < 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
					else if (ViewSpaceZaxis.z > 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
				}


				float deltaTransformation = active_delta_mouse * lightSpeed;

				currentlight->transformation.Translate(glm::vec3(NULL, NULL, deltaTransformation));

				currentlight->lightpos += glm::vec3(NULL, NULL, deltaTransformation);

				currentlight->originpoint += glm::vec3(NULL, NULL, deltaTransformation);

				LightPositions[CURRENT_LIGHT(currentselectedlight)] = currentlight->lightpos;

				currentlight->lightmodel = currentlight->transformation.GetModelMat4();

				handlelights(PBR_Shader);


			}

			else if (enablegizmo_p.first == X_GIZMO && enablegizmo_p.second == true)
			{

				double active_delta_mouse = NULL;

				glm::vec4 ViewSpaceXaxis = camera.cam_view * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
				float absX = glm::abs(ViewSpaceXaxis.x);
				float absY = glm::abs(ViewSpaceXaxis.y);
				float absZ = glm::abs(ViewSpaceXaxis.z);

				float max = glm::max(glm::max(absX, absY), absZ);

				if (max == absX)
				{
					active_delta_mouse = delta_mouse.x;

					if (ViewSpaceXaxis.x < 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
					else if (ViewSpaceXaxis.x > 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
				}
				else if (max == absY)
				{
					active_delta_mouse = delta_mouse.y;

					if (ViewSpaceXaxis.y < 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
					else if (ViewSpaceXaxis.y > 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
				}
				else if (max == absZ)
				{
					active_delta_mouse = delta_mouse.y;

					if (ViewSpaceXaxis.z < 0)
					{
						active_delta_mouse = -active_delta_mouse;
					}
					else if (ViewSpaceXaxis.z > 0)
					{
						active_delta_mouse = active_delta_mouse;
					}
				}



				float deltaTransformation = active_delta_mouse * lightSpeed;
				currentlight->transformation.Translate(glm::vec3(deltaTransformation, NULL, NULL));


				currentlight->lightpos += glm::vec3(deltaTransformation, NULL, NULL);

				currentlight->originpoint += glm::vec3(deltaTransformation, NULL, NULL);

				LightPositions[CURRENT_LIGHT(currentselectedlight)] = currentlight->lightpos;

				currentlight->lightmodel = currentlight->transformation.GetModelMat4();

				handlelights(PBR_Shader);
				
			}


		}
		


		return temp_mouse;
	}

	void CopyModelKeyboardAction(size_t currentselectedobj , GLuint shader , GLFWwindow *window ,std::vector<std::string> &logs ,glm::vec4 lightcolor , glm::vec3 lightpos)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && currentselectedobj >= 2)
		{

			static int counterf = NULL;

			if (counterf < 1)
			{

				CopyModel(currentselectedobj - 2);

				handlelights(shader);

				UseShaderProgram(shader);

				//glUniformMatrix4fv(glGetUniformLocation(defaultshader.GetID(), "model"), 1, GL_FALSE, glm::value_ptr(pyramidmodel));
				glUniform4f(glGetUniformLocation(shader, "lightColor1"), lightcolor.x, lightcolor.y, lightcolor.z, lightcolor.w);
				glUniform3f(glGetUniformLocation(shader, "lightpos1"), lightpos.x, lightpos.y, lightpos.z);

				std::string logtemp = "A new object is duplicated!";

				logs.push_back(logtemp);

			}


			counterf++;
		}

	}

	void DeleteModelKeyboardAction(int &currentselectedobj , GLFWwindow *window , std::vector<std::string>& logs)
	{

		if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && currentselectedobj >= 2)
		{
			uint* tempptr = GetModel(GetModelCount() - 1)->GetModelIDcounterptr();

			*GetModel(GetModelCount() - 1)->GetModelIDcounterptr() -= 1;

			DeleteModel(currentselectedobj - 2);

			if (GetModelCount() >= 1)
			{
				for (size_t i = 0; i < GetModelCount(); i++)
				{
					if (GetModel(i)->GetModelID() > 2 && GetModel(currentselectedobj - 3)->GetModelID() != GetModelCount() + 1)
					{
						*GetModel(i)->GetModelIDptr() -= 1;

					}

				}
			}

			std::cout << "NEW MODEL ID COUNTER: " << *tempptr << "\n";

			std::string logtemp = "A new object is deleted!";

			logs.push_back(logtemp);

			currentselectedobj = 0;

		}

	}

	void DeleteLightKeyboardAction(int& currentSelectedLight, GLFWwindow* window, std::vector<std::string>& logs , GLuint Shader)
	{

		if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && CURRENT_LIGHT(currentSelectedLight) >= 0)
		{
			DeleteLight(CURRENT_LIGHT(currentSelectedLight), Shader);
			std::string logtemp = "A light is deleted!";
			logs.push_back(logtemp);
			currentSelectedLight = 0;

		}

	}

	void FocusKeyboardAction(int currentSelectedObj, Camera& camera , int CameraLayout, GLFWwindow* window)
	{
		if (CameraLayout == CAMERA_LAYOUT_INDUSTRY_STANDARD)
		{
			if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && CURRENT_OBJECT(currentSelectedObj) >= 0)
			{
				camera.targetPosition = models[CURRENT_OBJECT(currentSelectedObj)]->transformation.Position;
			}
		}
	}

};


#endif // !LIGHTCLASS
