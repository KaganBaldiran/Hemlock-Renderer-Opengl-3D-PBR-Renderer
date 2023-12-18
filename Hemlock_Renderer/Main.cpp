#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <bitset>
#include "initialize.h"
//#include "Mesh.h"
#include "Camera.h"
#include "Buffers.h"
#include "glm/glm.hpp"
#include "Mesh.h"
#include "PickingTexture.h"
#include "post_process.h"
#include "Cubemap.h"
#include "Entity.h"
#include "Shadow_Map.h"
#include "UI.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_impl_glfw.h"
#include "nativefiledialog-extended-master/src/include/nfd.h"

#include "Log.h"
#include "StopWatch.h"
#include "Thread.h"
#include "Scene.h"
#include "NormalBaker.hpp"
#include <thread>

#define TARGET_FPS 144.0

const int windowwidth = 1000;
const int windowheight = 1000;

int main()
{
	GLFWwindow* window = initializeWindow(windowwidth, windowheight, "Hemlock Standalone Renderer");
	glViewport(0, 0, windowwidth, windowheight);

	Shader defaultshader("Shaders/vertsource.vert", "Shaders/fragsource.frag");
	Shader lightshader("Shaders/light.vert", "Shaders/light.frag");
	Shader Outlineshader("Shaders/Outlinefrag.vert", "Shaders/Outlinefrag.frag");
	Shader pickingshader("Shaders/picking.vert", "Shaders/picking.frag");
	Shader ShadowMapShader("Shaders/ShadowMap.vert", "Shaders/ShadowMap.frag");
	Shader FrameBufferShader("Shaders/framebuffer.vert", "Shaders/framebuffer.frag");
	Shader GbufferPassShader("Shaders/Gbuffer.vs", "Shaders/Gbuffer.fs");
	Shader HDRIShader("Shaders/HDRI.vs", "Shaders/HDRI.fs");
	Shader PickingBufferTextureShader("Shaders/PickingBufferTexture.vs", "Shaders/PickingBufferTexture.fs");
	Shader SSAOShader("Shaders/SSAO.vs", "Shaders/SSAO.fs");
	Shader SSAOblurShader("Shaders/SSAO.vs", "Shaders/SSAOblur.fs");
	Shader PBRShader("Shaders/PBR.vs", "Shaders/PBR.fs");
	Shader OmniShadowShader("Shaders/OmniShadow.vs", "Shaders/OmniShadow.gs", "Shaders/OmniShadow.fs");
	Shader ConvolutateCubeMapShader("Shaders/ConvolutationCubeMap.vs", "Shaders/ConvolutationCubeMap.fs");
	Shader PreFilterCubeMapShader("Shaders/PreFilterCubeMap.vs", "Shaders/PreFilterCubeMap.fs");
	Shader HighPolyBakingShader("Shaders/HighPolyNormalBake.vs", "Shaders/HighPolyNormalBake.fs");
	Shader ScreenSpaceilluminationShader("Shaders/SSGU.vs", "Shaders/SSGU.fs");
	Shader brdfLUTShader("Shaders/brdfLUT.vs", "Shaders/brdfLUT.fs");
	Shader PreviewShader("Shaders/ModelPreview.vs", "Shaders/ModelPreview.fs");

	scene scene;
	FBO screen_fbo;
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SSAO ssao({ (float)mode->width,(float)mode->height });
	CreateCustomFrameBuffer(screen_fbo, mode->width, mode->height);

	vector<std::string> cube_map_faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	std::unique_ptr<OmniShadowMap> OmnishadowMaps[MAX_LIGHT_COUNT];
	GLuint ShadowMapSize = 1024;
	for (size_t i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		OmnishadowMaps[i] = std::make_unique<OmniShadowMap>(ShadowMapSize, ShadowMapSize);
	}

	CubeMap Cubemap(cube_map_faces, "Shaders/CubeMap.vert", "Shaders/CubeMap.frag");

	Textures zeroPointButton("resources/buttons/zero_point_scaled_v2.png", 4, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, true);
	Textures GridButton("resources/buttons/grid_button.png", 4, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, true);
	Textures SplashScreenImage("resources/SplashScreenImage.png", 4, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, true);

	GBUFFER::gBuffer SceneGbuffer;
	NFD_Init();

	glUniform4f(glGetUniformLocation(PBRShader.GetID(), "colorpr"), 1.0f, 0.0f, 0.0f, 1.0f);

	Meshs grid = scene.SetGrid(lightshader.GetID());
	scene.ImportModel("resources/gizmo_arrow.obj", lightshader.GetID());

	glm::vec3 lightpos = glm::vec3(-0.5f, 0.9f, 0.5f);
	glm::vec4 lightcolor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightpos2 = glm::vec3(0.75f, 1.5f, 0.0f);
	glm::vec4 lightcolor2 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	scene.Addlight(lightpos, glm::vec3(2.0f, 2.0f, 2.0f), lightcolor, lightshader.GetID(), CUBE_LIGHT, DIRECTIONAL_LIGHT);
	scene.Addlight(lightpos2, glm::vec3(1.0f, 1.0f, 1.0f), lightcolor2, lightshader.GetID(), CUBE_LIGHT, SPOT_LIGHT);

	for (size_t i = 0; i < scene.GetModelCount(); i++)
	{
		scene.GetModel(i)->transformation.Scale(glm::vec3(0.05f, 0.05f, 0.05f));
	}

	scene.handlelights(PBRShader.GetID());
	UseShaderProgram(PBRShader.GetID());

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	float degree = NULL;

	pickingtexture pickingtex(mode->width, mode->height);
	pickingtexture pickingBuffertex(mode->width, mode->height);

	InitializeCameraMesh();
	AddCamera(glm::vec3(0.0f, 0.3f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f));

	int selection = NULL;
	int index = NULL;
	
	DATA::ObjectSelectionState SelectionState;

	Vec2<double> PrevMousePos = { NULL,NULL };

	std::pair<uint, bool>enablegizmo_p = { NULL , false };
	bool enablegizmo = false;

	bool allowclick = true;
	bool starttime = false;

	float time = NULL;
	glfwSetScrollCallback(window, scrollCallback);

	DATA::UIdataPack data;
	UI::InitNewUIwindow();
	UI::SetStyle(data);
	UI::SetPlatformBackEnd("#version 130", window);

	Model* selected_model = nullptr;
	int  selectedobjlock = NULL;
	//std::vector<std::string> logs;
	UI::InitLogs(logs);
	shadowmap ShadowMap(1024, 1024);

	ShadowMap.LightProjection(scene.LightPositions[0], ShadowMapShader.GetID(), window, scene.models, scene.globalscale, *Cameras[0], UI::current_viewport_size);

	scene.SetScreenQuads();

	stopwatch_t stopwatch;
	int FPS = 100;
	float DeltaFrameTime = (1.0f / FPS) * 1000;
	ThreadPool threads(1, 15);

	std::vector<uint> auto_rotate_on;
	int RenderPass = RENDER_PASS_COMBINED;

	Vec2<double> mousepos;
	int width, height;
	glfwSetDropCallback(window, UI::DropDownFile);
	SAVEFILE::ReadSaveFile("Preferences.json", data.saveFileData, logs);
	UI::SetPreferences(data);
	data.IsPreferencesFileEmpty = data.saveFileData.empty();

	data.ConvDiffCubeMap = ConvolutateCubeMap(Cubemap.GetCubeMapTexture(), ConvolutateCubeMapShader.GetID()).first;
	data.PrefilteredEnvMap = PreFilterCubeMap(Cubemap.GetCubeMapTexture(), PreFilterCubeMapShader.GetID()).first;
	const double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;

	data.brdfLUT = ComputeLUT(brdfLUTShader).first;
	
	WINDOWMANAGER::AddNewWindow<WINDOWMANAGER::OutlinerWindow>(scene, SelectionState, UI::current_color_sheme, PBRShader);
	WINDOWMANAGER::AddNewWindow<WINDOWMANAGER::LogsWindow>(UI::current_color_sheme);

	while (!glfwWindowShouldClose(window))
	{
		Camera& camera = *Cameras[ActiveCameraID];
		auto start_time = std::chrono::high_resolution_clock::now();

		glfwGetCursorPos(&*window, &mousepos.x, &mousepos.y);
		glfwGetWindowSize(&*window, &width, &height);
		UI::SetStyle(data);
		WindowSizeRecall(window, UI::current_viewport_size);
		UI::FindCurrentViewportSize(window);

		glClearColor(data.clear_color.x, data.clear_color.y, data.clear_color.z, data.clear_color.w);
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		UI::CreateNewFrame({width,height});
		camera.HandleInputs(window, UI::current_win_size, { width,height }, data.cameraLayout);
		UI::HandleSliderMaxValues(data, window);

		WINDOWMANAGER::WindowGarbageCollector();
		WINDOWMANAGER::UpdateWindows();

		UI::ConfigureUI(SelectionState.SelectedObject, data, scene, logs, PBRShader.GetID(), lightcolor, lightpos, window, auto_rotate_on,
			ShadowMap.GetShadowMapImage(), lightshader.GetID(), SelectionState.SelectedLight, threads,
			Cubemap, HDRIShader.GetID(), SplashScreenImage, RenderPass, camera, ConvolutateCubeMapShader.GetID(), PreFilterCubeMapShader.GetID());
		
		camera.updateMatrix(data.CameraFOV, 0.1f, 100.0f, window, { (int)(UI::UndockedViewPortRect.Max.x - UI::UndockedViewPortRect.Min.x) ,(int)(UI::UndockedViewPortRect.Max.y - UI::UndockedViewPortRect.Min.y) }, data.takesreenshot);
		UI::DropDownImportModel(PBRShader.GetID(), scene, logs,{width,height} , data,window, lightshader.GetID(),camera,RenderPass,
			HDRIShader.GetID(), ConvolutateCubeMapShader.GetID(), PreFilterCubeMapShader.GetID(),Cubemap,PreviewShader,threads);
		scene.DeleteModelKeyboardAction(SelectionState.SelectedObject, window, logs);
		scene.DeleteLightKeyboardAction(SelectionState.SelectedLight, window, logs, PBRShader.GetID());
		scene.FocusKeyboardAction(SelectionState.SelectedObject, camera, data.cameraLayout, window);
		UI::IncrementRotationDegree(data);
		Vec2<double> temp_mouse_pos = scene.UseGizmo(window, enablegizmo_p, PrevMousePos, camera, defaultshader.GetID(), PBRShader.GetID(), mousepos , SelectionState);
		UI::DoUIobjectTransformations(SelectionState.SelectedObject, scene, data);
		UI::HandleAutoRotation(SelectionState.SelectedObject, scene, auto_rotate_on);
		scene.CopyModelKeyboardAction(SelectionState.SelectedObject, defaultshader.GetID(), window, logs, lightcolor, lightpos);

		scene.DrawGbuffer(SceneGbuffer, GbufferPassShader.GetID(), camera,enablegizmo_p, PickingBufferTextureShader.GetID(), pickingBuffertex, data,SelectionState, { width,height });

		//ShadowMap.LightProjection(scene.LightPositions[0], ShadowMapShader.GetID(), window, scene.models, scene.globalscale, camera, UI::current_viewport_size);
		//scene.DrawShadowMap(&ShadowMap, ShadowMapShader.GetID(), camera, window, glm::vec4(data.clear_color.x, data.clear_color.y, data.clear_color.z, data.clear_color.w));
		//ShadowMap.Draw(scene, ShadowMapShader.GetID(), camera, window, glm::vec4(data.clear_color.x, data.clear_color.y, data.clear_color.z, data.clear_color.w));
		scene.DrawScene(data, OmnishadowMaps, OmniShadowShader.GetID(), camera, window, PBRShader, lightshader,
			screen_fbo, RenderPass, SelectionState.SelectedObject, Outlineshader, Cubemap, SelectionState.SelectedLight, enablegizmo_p,
			ssao, SSAOShader, SSAOblurShader, SceneGbuffer, width, height, grid, UI::current_viewport_size);


		scene.DrawScreenQuad(FrameBufferShader.GetID(), screen_fbo, SceneGbuffer,
			UI::current_win_size.Cast<float>(), UI::current_viewport_size.y, RenderPass, pickingtex,
			pickingshader.GetID(), pickingBuffertex, ssao, data.EnableSSAO, *window, data, camera,{UI::UndockedViewPortRect.Min.x,UI::UndockedViewPortRect.Min.y,
			UI::UndockedViewPortRect.Max.x , UI::UndockedViewPortRect.Max.y });

		//scene.DrawSSGUScreenQuad(ScreenSpaceilluminationShader.GetID(), screen_fbo.GetScreenImage(), SceneGbuffer,
			//UI::current_win_size.Cast<float>(), UI::current_viewport_size.y, RenderPass, pickingtex,
			//pickingshader.GetID(), pickingBuffertex, ssao, data.EnableSSAO, *window , camera);

		if (data.takesreenshot)
		{
			UseShaderProgram(0);
			scene.Takescreenshot(width, height, data.screenshotPathstr.c_str(), RenderPass, SceneGbuffer, screen_fbo);
			data.takesreenshot = false;
		}

		UI::DrawOnViewportSettings({ width,height }, RenderPass, *zeroPointButton.GetTexture(), *GridButton.GetTexture(), camera, data);
		UI::Render();

		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && !allowclick)
		{
			index = 0;
		}

		if (index == 0)
		{
			allowclick = true;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && allowclick)
		{
			if (SelectionState.SelectedObject >= 2)
			{
				UI::ReturnSelectedObjectData(data, scene.GetModel(CURRENT_OBJECT(SelectionState.SelectedObject))->UIprop);
			}

			allowclick = false;
			index = pickingtex.ReadPixel(mousepos.x, mousepos.y, { width,height }).ObjectID;
			glUniform1i(glGetUniformLocation(PBRShader.GetID(), "stencilindex"), index);
			if (index - 1 <= scene.GetModelCount())
			{
				SelectionState.SelectedObject = index;
				SelectionState.SelectedLight = NULL;
				SelectionState.SelectedGizmo = NULL;
			}
			else if (index - 1 >= scene.GetModelCount() && index - 1 <= scene.GetModelCount() + scene.lights.size())
			{
				SelectionState.SelectedLight = index;
				SelectionState.SelectedObject = NULL;
				SelectionState.SelectedGizmo = NULL;
			}
			else if (index == NULL)
			{
				SelectionState.SelectedLight = NULL;
				SelectionState.SelectedObject = NULL;
				SelectionState.SelectedGizmo = NULL;
			}
			if (SelectionState.SelectedObject >= 2)
			{
				UI::UseSelectedObjectData(data, scene.GetModel(CURRENT_OBJECT(SelectionState.SelectedObject))->UIprop);
			}
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{

			index = pickingtex.ReadPixel(mousepos.x, mousepos.y, { width,height }).ObjectID;;

			if (index - 1 >= scene.GetModelCount() + scene.lights.size())
			{
				SelectionState.SelectedGizmo = index;
			}

		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		UI::DoUIobjectReTransformations(SelectionState.SelectedObject, scene, data);
		UI::HandleReverseAutoTranslation(SelectionState.SelectedObject, scene, auto_rotate_on);

		PrevMousePos = temp_mouse_pos;

		auto end_time = std::chrono::high_resolution_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1e6;

		if (elapsed_time < TARGET_FRAME_TIME) {
			std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>((TARGET_FRAME_TIME - elapsed_time) * 1e6)));
		}
	}

	BindVAONull();
	BindVBONull();
	BindEBONull();
	UseShaderProgram(NULL);

	NFD_FreePath(data.outPath);

	DeleteShaderProgram(defaultshader.GetID());
	DeleteShaderProgram(lightshader.GetID());
	DeleteShaderProgram(Outlineshader.GetID());
	DeleteShaderProgram(pickingshader.GetID());
	DeleteShaderProgram(ShadowMapShader.GetID());
	DeleteShaderProgram(FrameBufferShader.GetID());
	DeleteShaderProgram(GbufferPassShader.GetID());
	DeleteShaderProgram(HDRIShader.GetID());
	DeleteShaderProgram(PickingBufferTextureShader.GetID());
	DeleteShaderProgram(SSAOShader.GetID());
	DeleteShaderProgram(SSAOblurShader.GetID());
	DeleteShaderProgram(PBRShader.GetID());
	DeleteShaderProgram(OmniShadowShader.GetID());
	DeleteShaderProgram(ConvolutateCubeMapShader.GetID());
	DeleteShaderProgram(PreFilterCubeMapShader.GetID());
	DeleteShaderProgram(HighPolyBakingShader.GetID());
	DeleteShaderProgram(ScreenSpaceilluminationShader.GetID());
	DeleteShaderProgram(brdfLUTShader.GetID());
	DeleteShaderProgram(PreviewShader.GetID());

	zeroPointButton.DeleteTexture();
	GridButton.DeleteTexture();
	SplashScreenImage.DeleteTexture();

	screen_fbo.Clear();

	glDeleteTextures(1, &data.brdfLUT);
	glDeleteTextures(1, &data.PrefilteredEnvMap);

	DisposeCameraMesh();
	DisposeCameras();

	UI::EndUI();
	WINDOWMANAGER::DisposeWindows();

	NFD_Quit();
	DeinitializeWindow();
	SAVEFILE::WriteSaveFile("Preferences.json", data.saveFileData, logs);
	return 0;
}