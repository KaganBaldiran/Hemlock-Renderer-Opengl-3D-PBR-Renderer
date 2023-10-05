#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <bitset>
#include "initialize.h"
//#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "Buffers.h"
#include "glm/glm.hpp"
#include "Mesh.h"
#include "Scene.h"
#include "PickingTexture.h"
#include "post_process.h"
#include "Cubemap.h"
#include "Entity.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_impl_glfw.h"
#include "nativefiledialog-extended-master/src/include/nfd.h"

#include"UI.h"
#include "Log.h"
#include "StopWatch.h"
#include "Thread.h"

#include "G_Buffer.h"

const int windowwidth = 1000;
const int windowheight = 1000;

int main()
{
    
	GLFWwindow*window = initializeWindow(windowwidth,windowheight , "Hemlock Standalone Renderer");

    glViewport(0, 0, windowwidth, windowheight);

    Shader defaultshader("Shaders/vertsource.vert", "Shaders/fragsource.frag");
    Shader lightshader("Shaders/light.vert", "Shaders/light.frag");
    Shader Outlineshader("Shaders/Outlinefrag.vert", "Shaders/Outlinefrag.frag");
    Shader pickingshader("Shaders/picking.vert", "Shaders/picking.frag");
    Shader ShadowMapShader("Shaders/ShadowMap.vert", "Shaders/ShadowMap.frag");
    Shader FrameBufferShader("Shaders/framebuffer.vert", "Shaders/framebuffer.frag");
    Shader GbufferPassShader("Shaders/Gbuffer.vs", "Shaders/Gbuffer.fs");
    Shader HDRIShader("Shaders/HDRI.vs", "Shaders/HDRI.fs");

    scene scene;

    FBO screen_fbo;

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    CreateCustomFrameBuffer(screen_fbo , mode->width, mode->height);

    vector<std::string> cube_map_faces
    {
        "resources/skybox/right.jpg",
        "resources/skybox/left.jpg",
        "resources/skybox/top.jpg",
        "resources/skybox/bottom.jpg",
        "resources/skybox/front.jpg",
        "resources/skybox/back.jpg"
    };

    CubeMap Cubemap(cube_map_faces, "Shaders/CubeMap.vert", "Shaders/CubeMap.frag");

    GBUFFER::gBuffer SceneGbuffer;

    NFD_Init();
    
    glUniform4f(glGetUniformLocation(defaultshader.GetID(), "colorpr"), 1.0f, 0.0f, 0.0f, 1.0f);

    Meshs grid = scene.SetGrid(lightshader.GetID());

    
    scene.ImportModel("resources/gizmo_arrow.obj", lightshader.GetID());
 
    glm::vec3 lightpos = glm::vec3(-0.5f, 0.9f, 0.5f);
    

    glm::vec4 lightcolor = glm::vec4(1.0f,1.0f,1.0f,1.0f);

    glm::vec3 lightpos2 = glm::vec3(0.75f, 1.5f, 0.0f);


    glm::vec4 lightcolor2 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    glm::vec3 lightpos3 = glm::vec3(-1.0f, 0.9f, -2.0f);
    glm::vec4 lightcolor3 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    scene.Addlight(lightpos, glm::vec3(2.0f, 2.0f, 2.0f), lightcolor, lightshader.GetID(), CUBE_LIGHT, DIRECTIONAL_LIGHT);
    scene.Addlight(lightpos2, glm::vec3(1.0f, 1.0f, 1.0f), lightcolor2, lightshader.GetID(), CUBE_LIGHT, SPOT_LIGHT);
   
    for (size_t i = 0; i < scene.GetModelCount(); i++)
    {
        scene.GetModel(i)->transformation.scale(glm::vec3(0.05f, 0.05f, 0.05f));
    }
   
    
    scene.handlelights(defaultshader.GetID());

    UseShaderProgram(defaultshader.GetID());

    //glUniformMatrix4fv(glGetUniformLocation(defaultshader.GetID(), "model"), 1, GL_FALSE, glm::value_ptr(pyramidmodel));
    glUniform4f(glGetUniformLocation(defaultshader.GetID(), "lightColor1"), lightcolor.x, lightcolor.y, lightcolor.z, lightcolor.w);
    glUniform3f(glGetUniformLocation(defaultshader.GetID(), "lightpos1"), lightpos.x, lightpos.y, lightpos.z);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);




    float degree = NULL;
    
    pickingtexture pickingtex(mode->width, mode->height);
    picking_technique pickingteq;
    


    Camera camera(windowwidth, windowheight, glm::vec3(0.0f, 0.3f, 2.0f));

    int selection = NULL;
    int index = NULL;
    int currentselectedobj = NULL;
    int currentselectedlight = NULL;
    int currentselectedgizmo = NULL;

    Vec2<double> PrevMousePos = { NULL,NULL };

    std::pair<uint, bool>enablegizmo_p = { NULL , false };
    bool enablegizmo = false;

    bool allowclick = true;
    //bool enablehighlight = false;
    bool starttime = false;

    float time = NULL;

    UI::UIdataPack data;
    UI::InitNewUIwindow();
    UI::SetStyle(data);
    UI::SetPlatformBackEnd("#version 130", window);

    Model* selected_model = nullptr;
    int  selectedobjlock = NULL;
    std::vector<std::string> logs;
    UI::InitLogs(logs);
    shadowmap ShadowMap(2048, 2048);

    ShadowMap.LightProjection(scene.LightPositions[0],ShadowMapShader.GetID(),window,scene.models,scene.globalscale,camera, UI::current_viewport_size);

    scene.SetScreenQuads();

    stopwatch_t stopwatch;
    int FPS = 100;
    float DeltaFrameTime = (1.0f / FPS) * 1000;
    ThreadPool threads(4, 15);

    std::vector<uint> auto_rotate_on;

    int RenderPass = RENDER_PASS_COMBINED;

	while (!glfwWindowShouldClose(window))
	{
            UI::SetStyle(data);

            WindowSizeRecall(window, UI::current_viewport_size);

            UI::FindCurrentViewportSize(window);

            glClearColor(data.clear_color.x, data.clear_color.y, data.clear_color.z, data.clear_color.w);
            glClearStencil(0); // this is the default value
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            UI::CreateNewFrame();


            camera.HandleInputs(window, UI::current_win_size);

            camera.updateMatrix(45.0f, 0.1f, 100.0f, window, UI::current_viewport_size);

            UI::HandleSliderMaxValues(data, window);

            UI::ConfigureUI(currentselectedobj, data, scene, logs, defaultshader.GetID(), lightcolor, lightpos, window, auto_rotate_on, 
                           ShadowMap.GetShadowMapImage(), lightshader.GetID(), currentselectedlight,threads,Cubemap,HDRIShader.GetID());
            
            scene.DeleteModelKeyboardAction(currentselectedobj, window, logs);

            scene.CopyModelKeyboardAction(currentselectedobj, defaultshader.GetID(), window, logs, lightcolor, lightpos);

            

            UI::IncrementRotationDegree(data);

            Vec2<double> temp_mouse_pos = scene.UseGizmo(window, currentselectedgizmo, currentselectedobj, enablegizmo_p, PrevMousePos, camera, currentselectedlight, defaultshader.GetID());

            UI::DoUIobjectTransformations(currentselectedobj, scene, data);

            UI::HandleAutoRotation(currentselectedobj, scene, auto_rotate_on);

            scene.DrawGbuffer(SceneGbuffer, GbufferPassShader.GetID(), camera, UI::current_win_size.Cast<float>(),*window);

            ShadowMap.LightProjection(scene.LightPositions[0], ShadowMapShader.GetID(), window, scene.models, scene.globalscale, camera, UI::current_viewport_size);

          
            scene.DrawShadowMap(&ShadowMap, ShadowMapShader.GetID(), camera, window, glm::vec4(data.clear_color.x, data.clear_color.y, data.clear_color.z, data.clear_color.w));

           
            WindowSizeRecall(window, UI::current_viewport_size);


            screen_fbo.Bind(GL_FRAMEBUFFER);
            glViewport(0, 0, screen_fbo.FboSize.x, screen_fbo.FboSize.y);

            if (RenderPass == RENDER_PASS_COMBINED)
            {

                glClearColor(data.clear_color.x, data.clear_color.y, data.clear_color.z, data.clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


                glEnable(GL_STENCIL_TEST);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


                //glEnable(GL_FRAMEBUFFER_SRGB);

                if (currentselectedobj >= 2)
                {
                    scene.DrawModelsWithOutline(defaultshader.GetID(), Outlineshader.GetID(), camera, currentselectedobj - 2, currentselectedobj, ShadowMap.GetShadowMapImage());

                }


                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                scene.RenderGrid(lightshader.GetID(), grid, camera);

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                if (data.takesreenshot)
                {
                    glClearColor(data.clear_color.x, data.clear_color.y, data.clear_color.z, data.clear_color.w);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                }

                if (data.render_cube_map)
                {
                    Cubemap.Draw(camera);
                }

                UseShaderProgram(defaultshader.GetID());



                for (int i = 0; i < scene.GetModelCount() + 1; i++) {
                    glStencilFunc(GL_ALWAYS, i + 1, -1);
                    if (i == 0)
                    {


                    }
                    if (i > 1)
                    {

                        ShadowMap.LightProjection(scene.LightPositions[0], defaultshader.GetID(), window, scene.models, scene.globalscale, camera, UI::current_viewport_size);

                        glUniform1i(glGetUniformLocation(defaultshader.GetID(), "enablehighlight"), data.enablehighlight);


                        scene.GetModel(i - 1)->transformation.SendUniformToShader(defaultshader.GetID(), "model");
                        scene.DrawModels(defaultshader.GetID(), camera, i - 1, ShadowMap.GetShadowMapImage(), Cubemap.GetCubeMapTexture());


                        glActiveTexture(GL_TEXTURE0);

                        UseShaderProgram(0);
                    }


                }

                if (data.renderlights)
                {

                    for (int i = 0; i < scene.lights.size(); i++) {
                        glStencilFunc(GL_ALWAYS, i + 1 + scene.GetModelCount() + 1, -1);

                        scene.lights[i]->Draw(lightshader.GetID(), camera);

                    }
                }
                if (CURRENT_OBJECT(currentselectedobj) >= NULL || scene.CURRENT_LIGHT(currentselectedlight) >= NULL)
                {
                    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                    glStencilMask(0xFF);
                    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                    for (int i = 0; i < 3; i++)
                    {
                        glStencilFunc(GL_ALWAYS, i + 1 + scene.GetModelCount() + 1 + scene.lights.size() + 2, -1);

                        scene.DrawGizmo(lightshader.GetID(), camera, i, currentselectedobj, enablegizmo_p, currentselectedlight);

                    }

                    glDepthFunc(GL_LESS);
                    glStencilMask(0xFF);
                    glStencilFunc(GL_ALWAYS, 1, 0xFF);
                    glEnable(GL_DEPTH_TEST);
                }

                //glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            if (data.takesreenshot)
            {
                Vec2<int> screensize;
                glfwGetWindowSize(window, &screensize.x, &screensize.y);

                scene.Takescreenshot(&ShadowMap, screensize.x, screensize.y, data.screenshotPathstr.c_str(),RenderPass,SceneGbuffer,screen_fbo);
                data.takesreenshot = false;
            }

            Vec2<double> mousepos;
            glfwGetCursorPos(&*window, &mousepos.x, &mousepos.y);

            int width, height;
            glfwGetWindowSize(&*window, &width, &height);

            UI::CalculateVirtualMouse(window);


            glBindFramebuffer(GL_FRAMEBUFFER, NULL);
           
            LOG("Current selected light: " << currentselectedlight);
            LOG("Current selected gizmo: " << currentselectedgizmo);
            LOG("INDEX: " << index);

            scene.DrawScreenQuad(FrameBufferShader.GetID(), screen_fbo.GetScreenImage(), SceneGbuffer,
                UI::current_win_size.Cast<float>(), UI::current_viewport_size.y, RenderPass, pickingtex, pickingshader.GetID(), *window);

            
            UI::DrawOnViewportSettings(*window, RenderPass);
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
                if (currentselectedobj >= 2)
                {
                    UI::ReturnSelectedObjectData(data, scene.GetModel(CURRENT_OBJECT(currentselectedobj))->UIprop);

                }

                allowclick = false;

                Vec2<double> mp = UI::CalculateVirtualMouse(window);

                index = pickingtex.ReadPixel(mousepos.x, mousepos.y, { width,height }).ObjectID;

                //std::cout << "index ID " << index << "\n";
                glUniform1i(glGetUniformLocation(defaultshader.GetID(), "stencilindex"), index);

                if (index - 1 <= scene.GetModelCount())
                {
                    currentselectedobj = index;
                    currentselectedlight = NULL;
                    currentselectedgizmo = NULL;
                }
                else if (index - 1 >= scene.GetModelCount() && index - 1 <= scene.GetModelCount() + scene.lights.size())
                {
                    currentselectedlight = index;
                    currentselectedobj = NULL;
                    currentselectedgizmo = NULL;

                }
                else if (index == NULL)
                {
                    currentselectedlight = NULL;
                    currentselectedobj = NULL;
                    currentselectedgizmo = NULL;

                }

                

                if (currentselectedobj >= 2)
                {
                    UI::UseSelectedObjectData(data, scene.GetModel(CURRENT_OBJECT(currentselectedobj))->UIprop);
                }

            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {

                Vec2<double> mp = UI::CalculateVirtualMouse(window);

                index = pickingtex.onMouse(mp.x, mp.y, { windowwidth,windowheight }, UI::current_win_size.Cast<float>());

                if (index - 1 >= scene.GetModelCount() + scene.lights.size())
                {
                    currentselectedgizmo = index;
                }

            }

            glfwSwapBuffers(window);
            glfwPollEvents();

            UI::DoUIobjectReTransformations(currentselectedobj, scene, data);
            UI::HandleReverseAutoTranslation(currentselectedobj, scene, auto_rotate_on);

            PrevMousePos = temp_mouse_pos;

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

    UI::EndUI();

    NFD_Quit();
    DeinitializeWindow();
	return 0;
}