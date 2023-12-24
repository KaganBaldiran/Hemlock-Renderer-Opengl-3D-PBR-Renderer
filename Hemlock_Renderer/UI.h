#pragma once

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <filesystem>
#include <string>

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_internal.h"
#include "Texture.h"
#include <tuple>
#include "SystemData.h"
#include "Thread.h"
#include "Cubemap.h"
#include "SaveFile.h"
#include "PreviewMaker.hpp"
#include "Window.hpp"

namespace UI
{

	Vec2<int> current_win_size((1000 / 4.0f), 1000 - 18);
	Vec2<int> current_viewport_size;
	ImRect UndockedViewPortRect;

	Vec2<float> ApplicationSettingSizes;
	Vec2<float> viewport_size;
	float image_ratio_divisor = NULL;
	ImFont* ImportedFont;

	ImVec2 MainMenuBarSize;

	UIcolorShemePack current_color_sheme;
	color_sheme_t chosen_color_sheme = GITHUB_STYLE_THEME;
	SAVEFILE::DropDownFileData DropdownFileData;

	void InitNewUIwindow()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		std::string FilePath = "resources\\Inter.ttf";
		
		try
		{
			ImGuiIO& io = ImGui::GetIO();
			ImportedFont = io.Fonts->AddFontFromFileTTF(FilePath.c_str(), 17.0f, NULL, io.Fonts->GetGlyphRangesDefault());
		}
		catch (const std::exception& e)
		{
			LOG_ERR("Cannot import font :: " << e.what() << " :: " << FilePath);
		}
	}

	void SetPlatformBackEnd(const char* version, GLFWwindow* window)
	{
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(version);
	}

	void CreateNewFrame(Vec2<int> WindowSize)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ImVec2 DockSpaceSize = { (float)(WindowSize.x - current_win_size.x),(float)current_win_size.y + 5 };

		ImGui::SetNextWindowPos({ (float)current_win_size.x,(float)MainMenuBarSize.y });
		ImGui::SetNextWindowSize(DockSpaceSize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("###DockSpace", 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | 
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::PopStyleVar();

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
			auto DockNode = ImGui::DockBuilderGetCentralNode(dockspace_id);
			UndockedViewPortRect = DockNode->Rect();

			static auto first_time = true;
			if (first_time)
			{
				first_time = false;

				ImGui::DockBuilderRemoveNode(dockspace_id); 
				ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);
				ImGui::DockBuilderSetNodeSize(dockspace_id, DockSpaceSize);

				auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
				auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, nullptr, &dockspace_id);
				auto dock_id_down= ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);

				ImGui::DockBuilderDockWindow("Logs", dock_id_right);
				ImGui::DockBuilderDockWindow("Outliner", dock_id_down);
				ImGui::DockBuilderFinish(dockspace_id);
			}
			
		}

	
		ImGui::End();
		ImGui::GetStyle().Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		//ImGui::GetStyle().Colors[ImGuiCol_Separator] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(3);
	}

	void SetPreferences(DATA::UIdataPack& data)
	{
		if (data.saveFileData.ViewportTheme != -1)
		{
			int savedViewportTheme = data.saveFileData.ViewportTheme;

			if (savedViewportTheme == DARK_THEME_ID)
			{
				chosen_color_sheme = DARK_THEME;
			}
			else if (savedViewportTheme == LIGHT_THEME_ID)
			{
				chosen_color_sheme = LIGHT_THEME;
			}
			else if (savedViewportTheme == BLUE_THEME_ID)
			{
				chosen_color_sheme = BLUE_THEME;
			}
			else if (savedViewportTheme == CUSTOM_THEME_ID)
			{
				chosen_color_sheme = CUSTOM_THEME;
			}
			else if (savedViewportTheme == PINK_THEME_ID)
			{
				chosen_color_sheme = PINK_THEME;
			}
			else if (savedViewportTheme == CYAN_THEME_ID)
			{
				chosen_color_sheme = CYAN_THEME;
			}
			else if (savedViewportTheme == ORANGE_THEME_ID)
			{
				chosen_color_sheme = ORANGE_THEME;
			}
			else if (savedViewportTheme == GREEN_THEME_ID)
			{
				chosen_color_sheme = GREEN_THEME;
			}
			else if (savedViewportTheme == NORDIC_THEME_ID)
			{
				chosen_color_sheme = NORDIC_THEME;
			}
			else if (savedViewportTheme == GITHUB_STYLE_THEME_ID)
			{
				chosen_color_sheme = GITHUB_STYLE_THEME;
			}
			else if (savedViewportTheme == INTELLIJ_STYLE_THEME_ID)
			{
				chosen_color_sheme = INTELLIJ_STYLE_THEME;
			}
			else if (savedViewportTheme == CHATGPT_LIGHT_THEME_ID)
			{
				chosen_color_sheme = CHATGPT_LIGHT_THEME;
			}
			else if (savedViewportTheme == SUNSET_THEME_ID)
			{
				chosen_color_sheme = SUNSET_THEME;
			}
			else if (savedViewportTheme == OCEANIC_THEME_ID)
			{
				chosen_color_sheme = OCEANIC_THEME;
			}
			else if (savedViewportTheme == VINTAGE_THEME_ID)
			{
				chosen_color_sheme = VINTAGE_THEME;
			}
			else if (savedViewportTheme == MINTY_FRESH_THEME_ID)
			{
				chosen_color_sheme = MINTY_FRESH_THEME;
			}
			else if (savedViewportTheme == ROYAL_PURPLE_THEME_ID)
			{
				chosen_color_sheme = ROYAL_PURPLE_THEME;
			}
		}
		if (data.saveFileData.CameraLayout != -1)
		{
			data.cameraLayout = data.saveFileData.CameraLayout;

			if (data.cameraLayout == CAMERA_LAYOUT_INDUSTRY_STANDARD)
			{
				data.cameraLayOutNameStr = "Industry Standard";
			}
			else if (data.cameraLayout == CAMERA_LAYOUT_FIRST_PERSON)
			{
				data.cameraLayOutNameStr = "First Person";
			}
		}
	}

	void SetStyle(DATA::UIdataPack& data)
	{
		ImGuiStyle& style = ImGui::GetStyle();


		current_color_sheme(chosen_color_sheme);

		data.clear_color = current_color_sheme.ViewportBackgroundColor;

		style.Colors[ImGuiCol_Text] = current_color_sheme.TextColor;
		style.Colors[ImGuiCol_WindowBg] = current_color_sheme.MenuBackgroundColor;
		style.Colors[ImGuiCol_Border] = current_color_sheme.BorderColor;
		style.Colors[ImGuiCol_MenuBarBg] = current_color_sheme.MenuBackgroundColor;

		style.FrameRounding = 5.0f;
		style.ItemSpacing = ImVec2(20, 10);
		style.ChildBorderSize = 1.3f;
		style.FrameBorderSize = 1.3f;
		style.Alpha = 0.9f;
		style.ScrollbarSize = 10.0f;
		style.WindowMinSize = ImVec2(0, 0);


	}

	void Render()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EndUI()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

	void UseSelectedObjectData(DATA::UIdataPack& data, UIproperties& obj_data)
	{
		data.autorotate = obj_data.autorotate;
		data.keepoldrotation = obj_data.keepoldrotation;
		data.maxmove = obj_data.maxmove;
		data.moveamount = obj_data.moveamount;
		data.newtreshold = obj_data.newtreshold;
		data.scaleamount = obj_data.scaleamount;
		data.maxscale = obj_data.maxscale;
		data.rotationamount = obj_data.rotationamount;
		data.degree = obj_data.degree;

	}

	void ReturnSelectedObjectData(DATA::UIdataPack& data, UIproperties& obj_data)
	{
		obj_data.autorotate = data.autorotate;
		obj_data.keepoldrotation = data.keepoldrotation;
		obj_data.maxmove = data.maxmove;
		obj_data.moveamount = data.moveamount;
		obj_data.newtreshold = data.newtreshold;
		obj_data.scaleamount = data.scaleamount;
		obj_data.maxscale = data.maxscale;
		obj_data.rotationamount = data.rotationamount;
		obj_data.degree = data.degree;
	}

	void InitLogs(std::vector<std::string>& logs)
	{
		int major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		std::string vendor = (const char*)glGetString(GL_VENDOR);
		vendor = "Vendor: " + vendor;

		std::string renderer = (const char*)glGetString(GL_RENDERER);
		renderer = "Renderer: " + renderer;

		std::string GLversion = "OpenGL Version: " + std::to_string(major) + "." + std::to_string(minor);

		std::string GLSLversion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		GLSLversion = "GLSL version: " + GLSLversion;

		cpu_id_t CPUdata;
		int flag = InitCpuid(CPUdata);

		logs.push_back(vendor);
		logs.push_back(renderer);
		logs.push_back(GLversion);
		logs.push_back(GLSLversion);

		try
		{
			std::string CPUInfoBrand(CPUdata.brand_str);
			logs.push_back("CPU: " + CPUInfoBrand);
		}
		catch (const std::exception& ex)
		{
			LOG_ERR("Unable to fetch cpu vendor! :: " << ex.what());
		}

	}

	void FrameBufferSizeCallBack(GLFWwindow* window)
	{
		Vec2<int> winsize;

		glfwGetWindowSize(window, &winsize.x, &winsize.y);

		int main_menu_height = 18;

		//current_win_size.x = winsize.x / 5.4f;
		current_win_size.y = winsize.y - main_menu_height;

		ImGui::SetNextWindowSize(ImVec2(current_win_size.x, current_win_size.y));
		ImGui::SetNextWindowPos(ImVec2(0, main_menu_height));

	}

	void FindCurrentViewportSize(GLFWwindow* window)
	{
		int height = NULL, width = NULL;
		glfwGetWindowSize(window, &width, &height);

		current_viewport_size = { width - current_win_size.x , current_win_size.y };

	}

	void DrawOnViewportSettings(Vec2<int> winsize, int& renderPass, GLuint zeroPointButton, GLuint gridButton, Camera& camera, DATA::UIdataPack& data)
	{
		static bool showDropdown = true;

		Vec2<float> viewportSettingsSize(UndockedViewPortRect.Max.x - UndockedViewPortRect.Min.x, current_win_size.y * 0.05f);
		ImGui::SetNextWindowPos(ImVec2(UndockedViewPortRect.Min.x, UndockedViewPortRect.Min.y));
		ImGui::SetNextWindowSize(ImVec2(viewportSettingsSize.x, viewportSettingsSize.y));

		ImGui::Begin("Viewport", (bool*)0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

		ImGui::SetCursorPos(ImVec2((viewportSettingsSize.x * 0.018f) + (viewportSettingsSize.y * 0.7f), viewportSettingsSize.y * 0.1f));

		ImVec2 uv0(0, 1);
		ImVec2 uv1(1, 0);

		if (ImGui::ImageButton((void*)(intptr_t)zeroPointButton, { viewportSettingsSize.y * 0.7f,viewportSettingsSize.y * 0.7f }, uv0, uv1))
		{
			camera.Position = glm::vec3(0.0f, 0.3f, 2.0f);
			camera.Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		}

		ImGui::SetCursorPos(ImVec2((viewportSettingsSize.x * 0.005f), viewportSettingsSize.y * 0.1f));

		if (ImGui::ImageButton((void*)(intptr_t)gridButton, { viewportSettingsSize.y * 0.7f,viewportSettingsSize.y * 0.7f }, uv0, uv1))
		{
			data.RenderGrid = !data.RenderGrid;
		}

		ImGui::SetCursorPos(ImVec2(viewportSettingsSize.x * 0.45f, viewportSettingsSize.y * 0.3f));

		std::string RenderPassUIText;

		if (renderPass == RENDER_PASS_NORMAL)
		{
			RenderPassUIText = "Normal";
		}
		else if (renderPass == RENDER_PASS_ALBEDO)
		{
			RenderPassUIText = "Albedo";
		}
		else if (renderPass == RENDER_PASS_POSITION)
		{
			RenderPassUIText = "Position";
		}
		else if (renderPass == RENDER_PASS_COMBINED)
		{
			RenderPassUIText = "Combined";
		}
		else if (renderPass == RENDER_PASS_ID)
		{
			RenderPassUIText = "ID";
		}
		else if (renderPass == RENDER_PASS_AO)
		{
			RenderPassUIText = "AO";
		}
		else if (renderPass == RENDER_PASS_SPECULAR)
		{
			RenderPassUIText = "Specular";
		}
		else if (renderPass == RENDER_PASS_WIREFRAME)
		{
			RenderPassUIText = "Wireframe";
		}

		if (ImGui::BeginCombo(RenderPassUIText.c_str(), "Select an option", ImGuiComboFlags_NoPreview))
		{

			if (ImGui::Selectable("Normal"))
			{
				renderPass = RENDER_PASS_NORMAL;
				showDropdown = false;
			}
			if (ImGui::Selectable("Albedo"))
			{

				renderPass = RENDER_PASS_ALBEDO;
				showDropdown = false;
			}
			if (ImGui::Selectable("Position"))
			{

				renderPass = RENDER_PASS_POSITION;
				showDropdown = false;
			}
			if (ImGui::Selectable("Combined"))
			{
				renderPass = RENDER_PASS_COMBINED;
				showDropdown = false;
			}
			if (ImGui::Selectable("ID"))
			{
				renderPass = RENDER_PASS_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("AO"))
			{
				renderPass = RENDER_PASS_AO;
				showDropdown = false;
			}
			if (ImGui::Selectable("Specular"))
			{
				renderPass = RENDER_PASS_SPECULAR;
				showDropdown = false;
			}
			if (ImGui::Selectable("Wireframe"))
			{
				renderPass = RENDER_PASS_WIREFRAME;
				showDropdown = false;
			}

			ImGui::EndCombo();
		}

		if (!ImGui::IsPopupOpen("##Dropdown"))
		{
			showDropdown = false;
		}

		ImGui::PopStyleColor();
		ImGui::SetCursorPos(ImVec2(viewportSettingsSize.x * 0.98f - (ImGui::GetFontSize() * 4), viewportSettingsSize.y * 0.3f));

		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

		ImGui::End();
	}

	void DoUIobjectTransformations(size_t currentselectedobj, scene& scene, DATA::UIdataPack& data)
	{
		if (currentselectedobj >= 2)
		{
			scene.GetModel(currentselectedobj - 2)->transformation.Translate(glm::vec3(data.moveamount.x, data.moveamount.y, data.moveamount.z));
			scene.GetModel(CURRENT_OBJECT(currentselectedobj))->dynamic_origin += glm::vec3(data.moveamount.x, data.moveamount.y, data.moveamount.z);
			if (data.scaleamount != 0.0f)
			{
				scene.GetModel(currentselectedobj - 2)->transformation.Scale(glm::vec3(data.scaleamount, data.scaleamount, data.scaleamount));

				scene.RecalculateObjectScales(currentselectedobj, glm::vec3(data.scaleamount, data.scaleamount, data.scaleamount));

			}
			scene.GetModel(currentselectedobj - 2)->transformation.Rotate(data.rotationamount, glm::vec3(0.0f, 1.0f, 0.0f));
			scene.GetModel(currentselectedobj - 2)->transformation.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
			scene.GetModel(currentselectedobj - 2)->transformation.Translate(-glm::vec3(0.0f, 0.0f, 0.0f));
		}

	}

	void DoUIobjectReTransformations(size_t currentselectedobj, scene& scene, DATA::UIdataPack& data)
	{
		if (currentselectedobj >= 2)
		{
			scene.GetModel(currentselectedobj - 2)->transformation.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
			scene.GetModel(currentselectedobj - 2)->transformation.Rotate(-data.rotationamount, glm::vec3(0.0f, 1.0f, 0.0f));
			scene.GetModel(currentselectedobj - 2)->transformation.Translate(-glm::vec3(0.0f, 0.0f, 0.0f));
			scene.GetModel(currentselectedobj - 2)->transformation.Scale(glm::vec3(1 / data.scaleamount, 1 / data.scaleamount, 1 / data.scaleamount));
			scene.GetModel(currentselectedobj - 2)->transformation.Translate(-glm::vec3(data.moveamount.x, data.moveamount.y, data.moveamount.z));
			scene.GetModel(CURRENT_OBJECT(currentselectedobj))->dynamic_origin -= glm::vec3(data.moveamount.x, data.moveamount.y, data.moveamount.z);
		}

	}

	void HandleSliderMaxValues(DATA::UIdataPack& data, GLFWwindow* window)
	{

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			if (data.moveamount.x == data.newtreshold.x)
			{
				data.maxmove.x = 2.0f * data.newtreshold.x;
				data.newtreshold.x = data.maxmove.x;
			}
			if (data.moveamount.y == data.newtreshold.y)
			{
				data.maxmove.y = 2.0f * data.newtreshold.y;
				data.newtreshold.y = data.maxmove.y;
			}
			if (data.moveamount.z == data.newtreshold.z)
			{
				data.maxmove.z = 2.0f * data.newtreshold.z;
				data.newtreshold.z = data.maxmove.z;
			}
			if (data.scaleamount == data.maxscale)
			{
				data.maxscale = 2.0f * data.maxscale;
				//maxscale = scaleamount;
			}
		}


	}

	void HandleAutoRotation(int& currentselectedobj, scene& scene, std::vector<uint>& auto_rotate_on)
	{
		for (size_t i = 0; i < auto_rotate_on.size(); i++)
		{
			if (auto_rotate_on.at(i) != CURRENT_OBJECT(currentselectedobj))
			{
				scene.GetModel(auto_rotate_on.at(i))->UIprop.degree = scene.GetModel(auto_rotate_on.at(i))->UIprop.rotationamount;
				scene.GetModel(auto_rotate_on.at(i))->UIprop.degree += 0.1f;

				if (scene.GetModel(auto_rotate_on.at(i))->UIprop.degree >= 360.0f)
				{
					scene.GetModel(auto_rotate_on.at(i))->UIprop.degree = NULL;
				}
				scene.GetModel(auto_rotate_on.at(i))->UIprop.rotationamount = scene.GetModel(auto_rotate_on.at(i))->UIprop.degree;
			}

		}

		for (size_t i = 0; i < auto_rotate_on.size(); i++)
		{
			if (auto_rotate_on.at(i) != CURRENT_OBJECT(currentselectedobj))
			{
				scene.GetModel(auto_rotate_on.at(i))->transformation.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
				scene.GetModel(auto_rotate_on.at(i))->transformation.Rotate(scene.GetModel(auto_rotate_on.at(i))->UIprop.rotationamount, glm::vec3(0.0f, 1.0f, 0.0f));
				scene.GetModel(auto_rotate_on.at(i))->transformation.Translate(-glm::vec3(0.0f, 0.0f, 0.0f));
			}

		}

	}

	void HandleReverseAutoTranslation(size_t currentselectedobj, scene& scene, std::vector<uint>& auto_rotate_on)
	{

		for (size_t i = 0; i < auto_rotate_on.size(); i++)
		{
			if (auto_rotate_on.at(i) != CURRENT_OBJECT(currentselectedobj))
			{
				scene.GetModel(auto_rotate_on.at(i))->transformation.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
				scene.GetModel(auto_rotate_on.at(i))->transformation.Rotate(-scene.GetModel(auto_rotate_on.at(i))->UIprop.rotationamount, glm::vec3(0.0f, 1.0f, 0.0f));
				scene.GetModel(auto_rotate_on.at(i))->transformation.Translate(-glm::vec3(0.0f, 0.0f, 0.0f));
			}
		}

	}

	void IncrementRotationDegree(DATA::UIdataPack& data)
	{
		if (data.autorotate)
		{
			data.degree += 0.1f;

			if (data.degree >= 360.0f)
			{
				data.degree = NULL;
			}

			data.rotationamount = data.degree;
		}
		if (!data.autorotate)
		{
			data.degree = data.rotationamount;
		}

	}

	void DropDownFile(GLFWwindow* window, int count, const char** paths)
	{
		int i;
		for (i = 0; i < count; i++)
		{
			LOG(paths[i]);
			DropdownFileData.DropDownFilePath = paths[i];
		}
		DropdownFileData.DropDownImport = true;
		const char* FileExtention = strrchr(DropdownFileData.DropDownFilePath.c_str(), '.');
		if (StrCmprCaseInsnstv(FileExtention, ".hml"))
		{
			const int Filetype = SAVEFILE::CheckHMLfileType(DropdownFileData.DropDownFilePath.c_str());
			LOG(Filetype);
			DropdownFileData.OverAllFileType = Filetype;
		}
		else if (StrCmprCaseInsnstv(FileExtention, ".png") || StrCmprCaseInsnstv(FileExtention, ".tga") ||
			StrCmprCaseInsnstv(FileExtention, ".jpeg") || StrCmprCaseInsnstv(FileExtention, ".jpg"))
		{
			DropdownFileData.OverAllFileType = IMAGE_FILE;
		}
		else if (StrCmprCaseInsnstv(FileExtention, ".obj") || StrCmprCaseInsnstv(FileExtention, ".fbx") || StrCmprCaseInsnstv(FileExtention, ".blend") ||
			StrCmprCaseInsnstv(FileExtention, ".gltf") || StrCmprCaseInsnstv(FileExtention, ".dae") || StrCmprCaseInsnstv(FileExtention, ".glb"))
		{
			DropdownFileData.OverAllFileType = OBJECT_FILE;
			DropdownFileData.MakeModelPreview = true;
		}
		else if (StrCmprCaseInsnstv(FileExtention, ".hdr"))
		{
			DropdownFileData.OverAllFileType = HDRI_FILE;
		}
		DropdownFileData.DropDownFileSize = GetFileSize<CONVERSION_MEGABYTE>(DropdownFileData.DropDownFilePath.c_str());
	}

	void DropDownMenu(Vec2<int> WindowSize, DATA::UIdataPack& data, scene& scene, const int& OverallFileType , Shader& PreviewShader , ThreadPool& threadpool)
	{
		static bool ModelImportError = false;
		static bool MeshImportError = false;
		static bool TextureUsageImportError = false;

		static ImVec2 DropDownMenuSize = { WindowSize.x / 2.0f , WindowSize.y / 2.0f };
		static ImVec2 DropDownMenuPosition = ImVec2((WindowSize.x / 2) - (DropDownMenuSize.x / 2), (WindowSize.y / 2) - (DropDownMenuSize.y / 2));
		const char* FileExtention = strrchr(DropdownFileData.DropDownFilePath.c_str(), '.');

		ImGui::SetNextWindowSize(ImVec2(DropDownMenuSize.x, DropDownMenuSize.y));
		ImGui::SetNextWindowPos(DropDownMenuPosition, ImGuiCond_FirstUseEver);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.ChildMenuColor));

		ImGui::Begin("Drop-Down assign", &DropdownFileData.DropDownImport, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
		float OriginalFontSize = ImGui::GetFont()->FontSize;
		float FontSizeMultiplier = ((DropDownMenuSize.x / WindowSize.x) + (DropDownMenuSize.y / WindowSize.y)) / 2.0f;
		ImGui::SetWindowFontScale(FontSizeMultiplier * 2.0f);

		ImGui::Text("File info: ");
		ImGui::Text("");
		ImGui::SameLine();
		ImGui::Text(("File Path: " + DropdownFileData.DropDownFilePath).c_str());
		ImGui::Text("");
		ImGui::SameLine();
		ImGui::Text(("Size: " + std::to_string(DropdownFileData.DropDownFileSize) + "mb").c_str());
		ImGui::Text("");
		ImGui::SameLine();
		ImGui::Text("Type:");
		ImGui::SameLine();
		if (DropdownFileData.OverAllFileType == HML_FILE)
		{
			ImGui::Text("HML file");
		}
		else if (DropdownFileData.OverAllFileType == HML_FILE_PACKED)
		{
			ImGui::Text("Packed HML file");
		}
		else if (DropdownFileData.OverAllFileType == IMAGE_FILE)
		{
			ImGui::Text("Texture file");
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Text("Assignment Options:");
			ImGui::Spacing();
						
			if (scene.GetModelCount() - 1 != 0)
			{
				if (ModelImportError)
				{
					ImGui::TextColored({ 1.0f,0.0f,0.0f,1.0f }, "Model isn't chosen!");
				}
				ImGui::Text("Select Object:");
				ImGui::SameLine();

				if (ImGui::BeginCombo("##Select Object Assign", "Select an object to assign"))
				{
					for (size_t i = 1; i < scene.GetModelCount(); i++)
					{
						if (ImGui::Selectable(scene.GetModel(i)->ModelName.c_str()))
						{
							DropdownFileData.ModelToAssignName = scene.GetModel(i)->ModelName;
							DropdownFileData.ModelToAssignID = i;
						}
					}
					ImGui::EndCombo();
				}

				if (DropdownFileData.ModelToAssignID > 0)
				{
					if (MeshImportError)
					{
						ImGui::TextColored({ 1.0f,0.0f,0.0f,1.0f }, "Mesh isn't chosen!");
					}
					ImGui::Text("Select Mesh:");
					ImGui::SameLine();

					if (ImGui::BeginCombo("##Select Mesh Assign", "Select a mesh to assign"))
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
						for (size_t i = 0; i < scene.GetModel(DropdownFileData.ModelToAssignID)->meshes.size(); i++)
						{
							if (ImGui::Selectable(scene.GetModel(DropdownFileData.ModelToAssignID)->meshes[i].meshname.c_str()))
							{
								DropdownFileData.MeshToAssignID = i;
							}
						}

						ImGui::PopStyleColor();
						ImGui::EndCombo();
					}

				}

				if (TextureUsageImportError)
				{
					ImGui::TextColored({ 1.0f,0.0f,0.0f,1.0f }, "Texture Usage isn't chosen!");
				}
				ImGui::Text("Texture Usage:");
				ImGui::SameLine();
				if (ImGui::BeginCombo("##Select Texture Usage", "Select Texture Usage"))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

					if (ImGui::Selectable("Diffuse Texture"))
					{
						DropdownFileData.ImportedTextureUsage = "texture_diffuse";
					}
					if (ImGui::Selectable("Roughness Texture"))
					{
						DropdownFileData.ImportedTextureUsage = "texture_specular";
					}
					if (ImGui::Selectable("Metallic Texture"))
					{
						DropdownFileData.ImportedTextureUsage = "texture_metalic";
					}
					if (ImGui::Selectable("Normal Texture"))
					{
						DropdownFileData.ImportedTextureUsage = "texture_normal";
					}
					ImGui::EndCombo();

					ImGui::PopStyleColor();
				}
			}
			else
			{
				ImGui::TextColored({ 1.0f,0.0f,0.0f,1.0f }, "There isn't an object to assign to!");
			}
		}
		else if (DropdownFileData.OverAllFileType == OBJECT_FILE)
		{
			ImGui::Text("3D Object file");

			if (DropdownFileData.MakeModelPreview)
			{
				try
				{
					//threadpool.enqueue(PREVIEW::CreatePreviewForModel, DropdownFileData.ModelPreview, DropdownFileData.DropDownFilePath.c_str(), PreviewShader , threadpool);
					//DropdownFileData.MakeModelPreview = false;
					PREVIEW::CreatePreviewForModel(DropdownFileData.ModelPreview, DropdownFileData.DropDownFilePath.c_str(), PreviewShader,&DropdownFileData.ImportedPreviewModel);
					DropdownFileData.MakeModelPreview = false;
				}
				catch (const std::exception& e)
				{
					LOG_ERR("Exception creating a model preview! :: " << e.what());
				}
			}

			ImVec2 uv0(0, 1); 
			ImVec2 uv1(1, 0);
			float ImageSize = DropDownMenuSize.x / 3.0f;
			ImGui::Text("Preview:");
			ImGui::Image((void*)(intptr_t)DropdownFileData.ModelPreview, ImVec2(ImageSize, ImageSize), uv0, uv1);
		}
		else if (DropdownFileData.OverAllFileType == HDRI_FILE)
		{
			ImGui::Text("HDRI file");
		}
		else
		{
			ImGui::TextColored({ 1.0f,0.0f,0.0f,1.0f }, "File type isn't compatible!");
		}

		const ImVec2 ButtonSize = { DropDownMenuSize.x * 0.1f , DropDownMenuSize.x * 0.05f };
		ImGui::SetCursorPos({ DropDownMenuSize.x * 0.99f - ButtonSize.x , DropDownMenuSize.y * 0.99f - ButtonSize.y });
		if (ImGui::Button("Import", ButtonSize))
		{
			bool AllowToProceed = true;
			if (OverallFileType < 0)
			{
				AllowToProceed = false;
			}
			else if (DropdownFileData.OverAllFileType == IMAGE_FILE)
			{
				if (DropdownFileData.ModelToAssignID < 0)
				{
					AllowToProceed = false;
					ModelImportError = true;
				}
				else { ModelImportError = false; }

				if (DropdownFileData.MeshToAssignID < 0)
				{
					AllowToProceed = false;
					MeshImportError = true;
				}
				else { MeshImportError = false; }

				if (DropdownFileData.ImportedTextureUsage.empty())
				{
					AllowToProceed = false;
					TextureUsageImportError = true;
				}
				else { TextureUsageImportError = false; }

			}

			if (AllowToProceed)
			{
				DropdownFileData.FinalizeImport = true;
				DropdownFileData.DropDownImport = false;
				ModelImportError = false;
				MeshImportError = false;
				TextureUsageImportError = false;

				if (!DropdownFileData.MakeModelPreview)
				{
					glDeleteTextures(1, &DropdownFileData.ModelPreview);
				}
				DropdownFileData.MakeModelPreview = true;
			}
		}

		DropDownMenuSize = ImGui::GetWindowSize();
		DropDownMenuPosition = ImGui::GetWindowPos();
		ImGui::End();
	}

	void DropDownImportModel(GLuint import_shader, scene& scene, std::vector<std::string>& logs, Vec2<int> WindowSize, DATA::UIdataPack& data, GLFWwindow* window,
		GLuint light_shader, Camera& camera, int& renderPass, GLuint HDRItoCubeMapShader, GLuint ConvolutateCubeMapShader, GLuint PrefilterHDRIShader, CubeMap& Cubemap , Shader& PreviewShader , ThreadPool& threadpool)
	{
		if (DropdownFileData.DropDownImport)
		{
			DropDownMenu(WindowSize, data, scene, DropdownFileData.OverAllFileType,PreviewShader, threadpool);

			if (!DropdownFileData.DropDownImport && !DropdownFileData.FinalizeImport && DropdownFileData.ImportedPreviewModel != nullptr)
			{
				*DropdownFileData.ImportedPreviewModel->GetModelIDcounterptr() -= 1;
				delete DropdownFileData.ImportedPreviewModel;
				DropdownFileData.ImportedPreviewModel = nullptr;
				LOG_INF("User pressed cancel :: clearing the model buffer!");
			}
		}
		if (DropdownFileData.FinalizeImport && !DropdownFileData.DropDownImport)
		{
			if (DropdownFileData.OverAllFileType == HML_FILE)
			{
				LOG_INF("Reading HML file... :: " << DropdownFileData.DropDownFilePath);
				SAVEFILE::ReadHMLfile(DropdownFileData.DropDownFilePath.c_str(), scene, import_shader, light_shader, data, camera, renderPass, logs, window, Cubemap, HDRItoCubeMapShader, ConvolutateCubeMapShader, PrefilterHDRIShader);
			}
			else if (DropdownFileData.OverAllFileType == HML_FILE_PACKED)
			{
				LOG_INF("Reading HML file... :: " << DropdownFileData.DropDownFilePath);
				SAVEFILE::ReadHMLfilePacked(DropdownFileData.DropDownFilePath.c_str(), scene, import_shader, light_shader, data, camera, renderPass, logs, window, Cubemap, HDRItoCubeMapShader, ConvolutateCubeMapShader, PrefilterHDRIShader);
			}
			else if (DropdownFileData.OverAllFileType == IMAGE_FILE)
			{
				if (DropdownFileData.ModelToAssignID >= 0 && DropdownFileData.MeshToAssignID >= 0 && !DropdownFileData.ImportedTextureUsage.empty())
				{
					LOG_INF("Reading Image file... :: " << DropdownFileData.DropDownFilePath);
					scene.ImportTextureIntoModel(DropdownFileData.DropDownFilePath.c_str(), DropdownFileData.ModelToAssignID,
						                         DropdownFileData.MeshToAssignID, DropdownFileData.ImportedTextureUsage.c_str());
				}
				else
				{
					LOG_ERR("Model , mesh or texture usage isn't choosen!");
				}
			}
			else if (DropdownFileData.OverAllFileType == OBJECT_FILE)
			{
				LOG_INF("Reading object file... :: " << DropdownFileData.DropDownFilePath);
				scene.ImportModel(DropdownFileData.ImportedPreviewModel);
				scene.handlelights(import_shader);
				UseShaderProgram(import_shader);
				scene.GetModel(scene.GetModelCount() - 1)->transformation.Scale(glm::vec3(0.05f, 0.05f, 0.05f));
				std::string logtemp = "A new object is imported!";
				logs.push_back(logtemp);

				DropdownFileData.ImportedPreviewModel = nullptr;
			}
			else if (DropdownFileData.OverAllFileType == HDRI_FILE)
			{
				ImportCubeMap(DropdownFileData.DropDownFilePath.c_str(), Cubemap, HDRItoCubeMapShader, ConvolutateCubeMapShader, PrefilterHDRIShader, data, logs);
			}

			DropdownFileData.DropDownImport = false;
			DropdownFileData.FinalizeImport = false;
			DropdownFileData.ModelToAssignName.clear();
			DropdownFileData.ImportedTextureUsage.clear();
			DropdownFileData.ModelToAssignID = -1;
			DropdownFileData.ModelToAssignID = -1;
			DropdownFileData.OverAllFileType = -1;
		}
	}

	void UIthemeComboBox(DATA::UIdataPack& data)
	{
		static bool showDropdown = false;

		if (ImGui::BeginCombo("Set Application Color Theme", "Select an option"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, current_color_sheme.TextColor);

			if (ImGui::Selectable("DARK THEME"))
			{

				chosen_color_sheme = DARK_THEME;
				data.saveFileData.ViewportTheme = DARK_THEME_ID;
				showDropdown = false;


			}
			if (ImGui::Selectable("LIGHT THEME"))
			{

				chosen_color_sheme = LIGHT_THEME;
				data.saveFileData.ViewportTheme = LIGHT_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("BLUE THEME"))
			{

				chosen_color_sheme = BLUE_THEME;
				data.saveFileData.ViewportTheme = BLUE_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("CUSTOM THEME"))
			{

				chosen_color_sheme = CUSTOM_THEME;
				data.saveFileData.ViewportTheme = CUSTOM_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("PINK THEME"))
			{

				chosen_color_sheme = PINK_THEME;
				data.saveFileData.ViewportTheme = PINK_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("CYAN THEME"))
			{
				chosen_color_sheme = CYAN_THEME;
				data.saveFileData.ViewportTheme = CYAN_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("ORANGE THEME"))
			{
				chosen_color_sheme = ORANGE_THEME;
				data.saveFileData.ViewportTheme = ORANGE_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("GREEN THEME"))
			{
				chosen_color_sheme = GREEN_THEME;
				data.saveFileData.ViewportTheme = GREEN_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("NORDIC THEME"))
			{
				chosen_color_sheme = NORDIC_THEME;
				data.saveFileData.ViewportTheme = NORDIC_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("GITHUB STYLED THEME"))
			{
				chosen_color_sheme = GITHUB_STYLE_THEME;
				data.saveFileData.ViewportTheme = GITHUB_STYLE_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("INTELLIJ STYLED THEME"))
			{
				chosen_color_sheme = INTELLIJ_STYLE_THEME;
				data.saveFileData.ViewportTheme = INTELLIJ_STYLE_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("CHATGPT LIGHT MODE STYLED THEME"))
			{
				chosen_color_sheme = CHATGPT_LIGHT_THEME;
				data.saveFileData.ViewportTheme = CHATGPT_LIGHT_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("SUNSET THEME"))
			{
				chosen_color_sheme = SUNSET_THEME;
				data.saveFileData.ViewportTheme = SUNSET_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("OCEANIC THEME"))
			{
				chosen_color_sheme = OCEANIC_THEME;
				data.saveFileData.ViewportTheme = OCEANIC_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("VINTAGE THEME"))
			{
				chosen_color_sheme = VINTAGE_THEME;
				data.saveFileData.ViewportTheme = VINTAGE_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("MINTY FRESH THEME"))
			{
				chosen_color_sheme = MINTY_FRESH_THEME;
				data.saveFileData.ViewportTheme = MINTY_FRESH_THEME_ID;
				showDropdown = false;
			}
			if (ImGui::Selectable("ROYAL PURPLE THEME"))
			{
				chosen_color_sheme = ROYAL_PURPLE_THEME;
				data.saveFileData.ViewportTheme = ROYAL_PURPLE_THEME_ID;
				showDropdown = false;
			}
			ImGui::PopStyleColor();

			ImGui::EndCombo();
		}


		if (!ImGui::IsPopupOpen("##Dropdown"))
		{
			showDropdown = false;
		}
	}

	void ConfigureUI(int& currentselectedobj, DATA::UIdataPack& data, scene& scene, std::vector<std::string>& logs, GLuint import_shader, glm::vec4 lightcolor, glm::vec3 lightpos, GLFWwindow* window, std::vector<uint>& auto_rotate_on, GLuint screen_image, GLuint light_shader, int& currentselectedlight, ThreadPool& threads, CubeMap& Cubemap, GLuint HDRItoCubeMapShader, Textures& SplashScreenImage, int& renderPass, Camera& camera, GLuint ConvolutateCubeMapShader, GLuint PrefilterHDRIShader)
	{
		ImGui::PushFont(ImportedFont);
		static bool importmodel_menu = false;
		static bool OpenHMLfile = false;
		static bool SaveHMLfile = false;
		static bool OpenHMLfilePacked = false;
		static bool SaveHMLfilePacked = false;
		static bool ApplicationMenuEnabled = false;

		Vec2<int> win_size = { NULL,NULL };
		glfwGetWindowSize(window, &win_size.x, &win_size.y);

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.MidMenuColor));

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{

				if (ImGui::MenuItem("Open HML file", "Ctrl+H+M"))
				{
					if (!OpenHMLfile)
					{

						OpenHMLfile = true;
						data.imported = false;

						nfdfilteritem_t filterItem[1] = { { "hml file", "hml" } };
						nfdresult_t result = NFD_OpenDialog(&data.outPath, filterItem, 1, NULL);
						if (result == NFD_OKAY)
						{
							puts("Successfully fetched the file path to read the hml file from!");
							puts(data.outPath);
						}
						else if (result == NFD_CANCEL)
						{
							puts("User pressed cancel.");
							data.imported = true;
							OpenHMLfile = false;
						}
						else
						{
							printf("Error: %s\n", NFD_GetError());
						}

					}
				}

				if (OpenHMLfile)
				{

					if (data.outPath != nullptr)
					{
						std::string path(data.outPath);

						for (size_t i = 0; i < path.size(); i++)
						{
							if (path.at(i) == '\\')
							{
								path.at(i) = '/';
							}
						}

						LOG_INF("Reading HML file... :: " << path);
						SAVEFILE::ReadHMLfile(path.c_str(), scene, import_shader, light_shader, data, camera, renderPass, logs, window, Cubemap, HDRItoCubeMapShader, ConvolutateCubeMapShader, PrefilterHDRIShader);

						if (data.saveFileData.RecentProjects.size() < 5)
						{
							data.saveFileData.RecentProjects.push_back({ path, HML_FILE });
						}
						else if (data.saveFileData.RecentProjects.size() >= 5)
						{
							for (size_t i = 0; i < data.saveFileData.RecentProjects.size() - 1; i++)
							{
								data.saveFileData.RecentProjects[i] = data.saveFileData.RecentProjects[i + 1];
							}
							data.saveFileData.RecentProjects[data.saveFileData.RecentProjects.size() - 1] = { path, HML_FILE };
						}

						data.imported = true;
						OpenHMLfile = false;
					}


				}

				if (ImGui::MenuItem("Open Packed HML file", "Ctrl+H+M"))
				{
					if (!OpenHMLfilePacked)
					{

						OpenHMLfilePacked = true;
						data.imported = false;

						nfdfilteritem_t filterItem[1] = { { "hml file", "hml" } };
						nfdresult_t result = NFD_OpenDialog(&data.outPath, filterItem, 1, NULL);
						if (result == NFD_OKAY)
						{
							puts("Successfully fetched the file path to read the packed hml file from!");
							puts(data.outPath);
						}
						else if (result == NFD_CANCEL)
						{
							puts("User pressed cancel.");
							data.imported = true;
							OpenHMLfilePacked = false;
						}
						else
						{
							printf("Error: %s\n", NFD_GetError());
						}

					}
				}

				if (OpenHMLfilePacked)
				{

					if (data.outPath != nullptr)
					{
						std::string path(data.outPath);

						for (size_t i = 0; i < path.size(); i++)
						{
							if (path.at(i) == '\\')
							{
								path.at(i) = '/';
							}
						}

						LOG_INF("Reading HML file... :: " << path);
						SAVEFILE::ReadHMLfilePacked(path.c_str(), scene, import_shader, light_shader, data, camera, renderPass, logs, window, Cubemap, HDRItoCubeMapShader, ConvolutateCubeMapShader, PrefilterHDRIShader);

						if (data.saveFileData.RecentProjects.size() < 5)
						{
							data.saveFileData.RecentProjects.push_back({ path, HML_FILE_PACKED });
						}
						else if (data.saveFileData.RecentProjects.size() >= 5)
						{
							for (size_t i = 0; i < data.saveFileData.RecentProjects.size() - 1; i++)
							{
								data.saveFileData.RecentProjects[i] = data.saveFileData.RecentProjects[i + 1];
							}
							data.saveFileData.RecentProjects[data.saveFileData.RecentProjects.size() - 1] = { path, HML_FILE_PACKED };
						}

						data.imported = true;
						OpenHMLfilePacked = false;
					}

				}



				if (ImGui::MenuItem("Save HML file", "Ctrl+H+S"))
				{
					if (!SaveHMLfile)
					{
						SaveHMLfile = true;
						data.imported = false;

						nfdfilteritem_t filterItem[1] = { { "hml file", "hml" } };
						nfdresult_t result = NFD_SaveDialog(&data.outPath, filterItem, 1, NULL, "Project");
						if (result == NFD_OKAY)
						{
							puts("Successfully fetched the file path to save the hml file!");
							puts(data.outPath);
						}
						else if (result == NFD_CANCEL)
						{
							puts("User pressed cancel.");
							data.imported = true;
							SaveHMLfile = false;
						}
						else
						{
							printf("Error: %s\n", NFD_GetError());
						}

					}
				}
				if (SaveHMLfile)
				{

					if (data.outPath != nullptr)
					{
						std::string path(data.outPath);

						for (size_t i = 0; i < path.size(); i++)
						{
							if (path.at(i) == '\\')
							{
								path.at(i) = '/';
							}
						}

						LOG_INF("Writing HML file... :: " << path);
						SAVEFILE::WriteHMLfile(path.c_str(), scene, data, camera, renderPass, logs);

						data.imported = true;
						SaveHMLfile = false;
					}

				}

				if (ImGui::MenuItem("Save Packed HML file", "Ctrl+H+S"))
				{
					if (!SaveHMLfilePacked)
					{

						SaveHMLfilePacked = true;
						data.imported = false;

						nfdfilteritem_t filterItem[1] = { { "hml file", "hml" } };
						nfdresult_t result = NFD_SaveDialog(&data.outPath, filterItem, 1, NULL, "Project");
						if (result == NFD_OKAY)
						{
							puts("Successfully fetched the file path to save the hml packed file!");
							puts(data.outPath);
						}
						else if (result == NFD_CANCEL)
						{
							puts("User pressed cancel.");
							data.imported = true;
							SaveHMLfilePacked = false;
						}
						else
						{
							printf("Error: %s\n", NFD_GetError());
						}

					}
				}

				if (SaveHMLfilePacked)
				{

					if (data.outPath != nullptr)
					{
						std::string path(data.outPath);

						for (size_t i = 0; i < path.size(); i++)
						{
							if (path.at(i) == '\\')
							{
								path.at(i) = '/';
							}
						}

						LOG_INF("Writing HML file... :: " << path);
						SAVEFILE::WriteHMLfilePacked(path.c_str(), scene, data, camera, renderPass, logs);

						data.imported = true;
						SaveHMLfilePacked = false;
					}


				}


				if (ImGui::MenuItem("Import Model", "Ctrl+M"))
				{
					if (!importmodel_menu)
					{

						importmodel_menu = true;
						data.imported = false;

						nfdfilteritem_t filterItem[3] = { { "3D object files", "obj,fbx" }, { "obj", "obj" } , {"fbx","fbx"} };
						nfdresult_t result = NFD_OpenDialog(&data.outPath, filterItem, 3, NULL);
						if (result == NFD_OKAY)
						{
							puts("Success!");
							puts(data.outPath);
						}
						else if (result == NFD_CANCEL)
						{
							puts("User pressed cancel.");
							data.imported = true;
							importmodel_menu = false;
						}
						else
						{
							printf("Error: %s\n", NFD_GetError());
						}


					}
				}

				if (importmodel_menu)
				{

					if (data.outPath != nullptr)
					{
						std::string path(data.outPath);

						for (size_t i = 0; i < path.size(); i++)
						{
							if (path.at(i) == '\\')
							{
								path.at(i) = '/';
							}
						}

						std::cout << "NEW PATH: " << path << "\n";

						std::string temp(path);
						scene.ImportModel(temp, import_shader);
						scene.handlelights(import_shader);

						UseShaderProgram(import_shader);

						//glUniformMatrix4fv(glGetUniformLocation(defaultshader.GetID(), "model"), 1, GL_FALSE, glm::value_ptr(pyramidmodel));
						glUniform4f(glGetUniformLocation(import_shader, "lightColor1"), lightcolor.x, lightcolor.y, lightcolor.z, lightcolor.w);
						glUniform3f(glGetUniformLocation(import_shader, "lightpos1"), lightpos.x, lightpos.y, lightpos.z);


						scene.GetModel(scene.GetModelCount() - 1)->transformation.Scale(glm::vec3(0.05f, 0.05f, 0.05f));

						std::string logtemp = "A new object is imported!";

						logs.push_back(logtemp);

						data.imported = true;
						importmodel_menu = false;
					}


				}
				if (ImGui::MenuItem("Save Screenshot", "Ctrl + C"))
				{

					if (!data.takesreenshot)
					{


						data.takesreenshot = true;


					}
					if (data.takesreenshot)
					{
						nfdfilteritem_t filterItem[2] = { { "Image Files", "png" }, { "png", "png" } };
						nfdresult_t result = NFD_SaveDialog(&data.screenshotPath, filterItem, 2, NULL, NULL);
						if (result == NFD_OKAY)
						{
							puts("Success!");
							puts(data.screenshotPath);
						}
						else if (result == NFD_CANCEL)
						{
							puts("User pressed cancel.");
							data.takesreenshot = false;
						}
						else
						{
							printf("Error: %s\n", NFD_GetError());
						}

						if (data.screenshotPath != nullptr)
						{

							std::string path(data.screenshotPath);

							for (size_t i = 0; i < path.size(); i++)
							{
								if (path.at(i) == '\\')
								{
									path.at(i) = '/';
								}
							}

							data.screenshotPathstr = path;
						}

					}


				}


				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{

				if (ImGui::MenuItem("Undo", "Ctrl+Z")) { /* Do something */ }
				if (ImGui::MenuItem("Redo", "Ctrl+Y")) { /* Do something */ }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Add"))
			{
				if (ImGui::BeginMenu("Lights"))
				{

					if (ImGui::MenuItem("Directional Light", "Ctrl+L+D"))
					{
						scene.Addlight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), light_shader, CUBE_LIGHT, DIRECTIONAL_LIGHT);
						scene.handlelights(import_shader);
					}
					if (ImGui::MenuItem("Spot Light", "Ctrl+L+S"))
					{
						scene.Addlight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), light_shader, CUBE_LIGHT, SPOT_LIGHT);
						scene.handlelights(import_shader);
					}
					if (ImGui::MenuItem("Point Light", "Ctrl+L+P"))
					{
						scene.Addlight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), light_shader, CUBE_LIGHT, POINT_LIGHT);
						scene.handlelights(import_shader);
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Camera", "Ctrl+A+C"))
				{
					AddCamera({ 0.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f });
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("About"))
			{
				if (ImGui::MenuItem("Application Settings", "Ctrl+A+S"))
				{
					ApplicationMenuEnabled = true;
					data.SplashScreenEnabled = false;
				}
				if (ImGui::MenuItem("Splash Screen", "Ctrl+D+S"))
				{
					ApplicationMenuEnabled = false;
					data.SplashScreenEnabled = true;
				}

				ImGui::EndMenu();
			}

			MainMenuBarSize = ImGui::GetWindowSize();

			ImGui::EndMainMenuBar();
		}

		ImGui::PopStyleColor();

		if (ApplicationMenuEnabled)
		{
			ApplicationSettingSizes = { win_size.x / 2.0f , win_size.y / 2.0f };
			ImVec2 ApplicationSettingPosition = ImVec2((win_size.x / 2) - (ApplicationSettingSizes.x / 2), (win_size.y / 2) - (ApplicationSettingSizes.y / 2));

			ImGui::SetNextWindowSize(ImVec2(ApplicationSettingSizes.x, ApplicationSettingSizes.y));
			ImGui::SetNextWindowPos(ApplicationSettingPosition, ImGuiCond_FirstUseEver);

			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.ChildMenuColor));

			ImGui::Begin("Application Settings", &ApplicationMenuEnabled, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);

			ApplicationSettingPosition = ImGui::GetWindowPos();

			ImVec2 Max = ImVec2(ApplicationSettingPosition.x + (ApplicationSettingSizes.x), ApplicationSettingPosition.y + (ApplicationSettingSizes.y));
			ImVec2 Min = ImVec2(ApplicationSettingPosition.x - (ApplicationSettingSizes.x), ApplicationSettingPosition.y - (ApplicationSettingSizes.y));

			if (!ImGui::IsMouseHoveringRect(Min, Max) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{

				ApplicationMenuEnabled = false;

			}

			static bool HDRISettingShowDropdown = false;

			ImGui::Text(std::to_string(data.CubeMapSize).c_str());
			ImGui::SameLine();
			if (ImGui::BeginCombo("Cubemap(HDRI) Texture Size", "Select an option", ImGuiComboFlags_NoPreview))
			{
				if (ImGui::Selectable("512x512"))
				{
					data.CubeMapSize = 512;
					HDRISettingShowDropdown = false;
				}
				if (ImGui::Selectable("1024x1024"))
				{
					data.CubeMapSize = 1024;
					HDRISettingShowDropdown = false;
				}
				if (ImGui::Selectable("2048x2048"))
				{
					data.CubeMapSize = 2048;
					HDRISettingShowDropdown = false;
				}
				if (ImGui::Selectable("4096x4096"))
				{
					data.CubeMapSize = 4096;
					HDRISettingShowDropdown = false;
				}

				ImGui::EndCombo();
			}

			static bool CameraLayoutSettingShowDropdown = false;

			if (ImGui::BeginCombo("Camera Layout", data.cameraLayOutNameStr.c_str()))
			{
				if (ImGui::Selectable("Industry Standard"))
				{
					data.cameraLayout = CAMERA_LAYOUT_INDUSTRY_STANDARD;
					data.saveFileData.CameraLayout = CAMERA_LAYOUT_INDUSTRY_STANDARD;
					CameraLayoutSettingShowDropdown = false;
					data.cameraLayOutNameStr = "Industry Standard";
				}
				if (ImGui::Selectable("First Person"))
				{
					data.cameraLayout = CAMERA_LAYOUT_FIRST_PERSON;
					data.saveFileData.CameraLayout = CAMERA_LAYOUT_FIRST_PERSON;
					CameraLayoutSettingShowDropdown = false;
					data.cameraLayOutNameStr = "First Person";
				}


				ImGui::EndCombo();
			}

			UIthemeComboBox(data);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (ApplicationSettingSizes.x * 0.03f), p.y), ImVec2(p.x + (ApplicationSettingSizes.x * 0.94f), p.y), ImGui::GetColorU32(current_color_sheme.ChildMenuColor), 2.0f);

			ImGui::Spacing();
			ImGui::Spacing();

			std::string firstString("Hemlock Renderer is an Open-Source project");
			float Position = ApplicationSettingSizes.x * 0.5f - (ImGui::CalcTextSize(firstString.c_str()).x * 0.5f);
			ImGui::SetCursorPosX(Position);
			ImGui::Text("Hemlock Renderer is an Open-Source project");
			ImGui::SetCursorPosX(Position);
			ImGui::Text("For more details 'Github/KaganBaldiran'");
			ImGui::SetCursorPosX(Position);
			ImGui::Text("Hemlock Renderer 2023");

			ImGui::End();

			ImGui::PopStyleColor();
		}

		if (data.SplashScreenEnabled)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			Vec2<float> ScreenScaleRatio(win_size.x / (float)mode->width, win_size.y / (float)mode->height);

			ApplicationSettingSizes = { (SplashScreenImage.GetTextureWidth() * ScreenScaleRatio.x * 0.5f),
									  (SplashScreenImage.GetTextureHeight() * ScreenScaleRatio.x * 0.5f) + (win_size.y * 0.2f) };
			ImVec2 ApplicationSettingPosition = ImVec2((win_size.x / 2) - (ApplicationSettingSizes.x / 2), (win_size.y / 2) - (ApplicationSettingSizes.y / 2));


			ImGui::SetNextWindowSize(ImVec2(ApplicationSettingSizes.x, ApplicationSettingSizes.y));
			ImGui::SetNextWindowPos(ApplicationSettingPosition, ImGuiCond_FirstUseEver);

			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.ChildMenuColor));

			ImGui::Begin("Splash Screen", &ApplicationMenuEnabled, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);

			ApplicationSettingPosition = ImGui::GetWindowPos();

			ImVec2 Max = ImVec2(ApplicationSettingPosition.x + (ApplicationSettingSizes.x), ApplicationSettingPosition.y + (ApplicationSettingSizes.y));
			ImVec2 Min = ImVec2(ApplicationSettingPosition.x - (ApplicationSettingSizes.x), ApplicationSettingPosition.y - (ApplicationSettingSizes.y));

			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)(intptr_t)*SplashScreenImage.GetTexture(), ImVec2(SplashScreenImage.GetTextureWidth() * ScreenScaleRatio.x * 0.5f, SplashScreenImage.GetTextureHeight() * ScreenScaleRatio.x * 0.5f), uv0, uv1);

			if (data.IsPreferencesFileEmpty)
			{
				static bool CameraLayoutSettingShowDropdown = false;

				if (ImGui::BeginCombo("Camera Layout", data.cameraLayOutNameStr.c_str()))
				{
					if (ImGui::Selectable("Industry Standard"))
					{
						data.cameraLayout = CAMERA_LAYOUT_INDUSTRY_STANDARD;
						data.saveFileData.CameraLayout = CAMERA_LAYOUT_INDUSTRY_STANDARD;
						CameraLayoutSettingShowDropdown = false;
						data.cameraLayOutNameStr = "Industry Standard";
					}
					if (ImGui::Selectable("First Person"))
					{
						data.cameraLayout = CAMERA_LAYOUT_FIRST_PERSON;
						data.saveFileData.CameraLayout = CAMERA_LAYOUT_FIRST_PERSON;
						CameraLayoutSettingShowDropdown = false;
						data.cameraLayOutNameStr = "First Person";
					}


					ImGui::EndCombo();
				}

				UIthemeComboBox(data);
			}
			else
			{
				float RecentPrPosition = ApplicationSettingSizes.x * 0.1f;
				ImGui::SetCursorPosX(RecentPrPosition);
				ImGui::Text("Recent Projects");

				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (ApplicationSettingSizes.x * 0.03f), p.y), ImVec2(p.x + (ApplicationSettingSizes.x * 0.94f), p.y), ImGui::GetColorU32(current_color_sheme.ChildMenuColor), 2.0f);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, current_color_sheme.ChildMenuColor);
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

				ImGui::Spacing();

				for (size_t i = 0; i < data.saveFileData.RecentProjects.size(); i++)
				{
					std::string buttonLabel(strrchr(data.saveFileData.RecentProjects[i].first.c_str(), '/'));
					buttonLabel = buttonLabel.substr(1);
					ImGui::SetCursorPosX(RecentPrPosition);
					if (ImGui::Button(buttonLabel.c_str(), { ImGui::CalcTextSize((buttonLabel + " ").c_str()).x , ImGui::GetFontSize() * 2.0f }))
					{
						if (data.saveFileData.RecentProjects[i].second == HML_FILE)
						{
							SAVEFILE::ReadHMLfile(data.saveFileData.RecentProjects[i].first.c_str(), scene, import_shader, light_shader, data, camera, renderPass, logs, window, Cubemap, HDRItoCubeMapShader, ConvolutateCubeMapShader, PrefilterHDRIShader);
						}
						else if (data.saveFileData.RecentProjects[i].second == HML_FILE_PACKED)
						{
							SAVEFILE::ReadHMLfilePacked(data.saveFileData.RecentProjects[i].first.c_str(), scene, import_shader, light_shader, data, camera, renderPass, logs, window, Cubemap, HDRItoCubeMapShader, ConvolutateCubeMapShader, PrefilterHDRIShader);
						}

						data.SplashScreenEnabled = false;
					}
				}

				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

			}

			if (!ImGui::IsMouseHoveringRect(Min, Max) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				data.SplashScreenEnabled = false;
			}

			ImGui::End();

			ImGui::PopStyleColor();
		}

		FrameBufferSizeCallBack(window);

		const float MainTabCount = 3.0f;
		ImVec2 MainTabSize(current_win_size.x / MainTabCount, current_win_size.y * 0.05f);

		ImGui::SetNextWindowPos({ 0,MainMenuBarSize.y });

		ImGui::Begin("Settings", (bool*)0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
		ImGui::SetCursorPos(ImVec2(0, MainMenuBarSize.y));

		if (ImGui::Button("Object Properties", MainTabSize))
		{
			data.Outlinerbutton = false;
			data.propertiesbutton = true;
			data.logbutton = false;
		}

		const ImVec2 ChildMenuSize(current_win_size.x / 1.01f, current_win_size.y * 0.91f);
		const ImVec2 ChildMenuPos(current_win_size.x * 0.005f, current_win_size.y * 0.03f + MainTabSize.y);

		if (data.propertiesbutton)
		{
			if (currentselectedobj >= 2)
			{

				if (currentselectedobj == scene.GetModel(currentselectedobj - 2)->GetModelID())
				{
					ImGui::SetCursorPos(ChildMenuPos);

					//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(ImVec4(45 / 255.0, 55 / 255.0, 71 / 255.0, 28 / 255.0)));

					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.MidMenuColor));
					ImGui::BeginChildFrame(66, ChildMenuSize, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);
					ImGui::PopStyleColor();

					//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(ImVec4(72 / 255.0, 89 / 255.0, 115 / 255.0, 45 / 255.0)));
					//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.MenuBackgroundColor));
					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.ChildMenuColor));
					ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 40.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 3.0f);

					if (ImGui::CollapsingHeader("Model General Settings", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed))
					{
						ImGui::BeginChildFrame(3, ImVec2(ChildMenuSize.x * 0.98f, ChildMenuSize.y * 0.40f), ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);
						static float f = 0.0f;
						static int counter = 0;

						//ImGui::SetCursorPos(ImVec2(10, 10));
						ImGui::Checkbox("Show light meshes", &data.renderlights);
						ImGui::Checkbox("Auto Rotate", &data.autorotate);

						if (data.autorotate)
						{
							bool permitted = true;

							for (size_t i = 0; i < auto_rotate_on.size(); i++)
							{
								if (auto_rotate_on.at(i) == CURRENT_OBJECT(currentselectedobj))
								{
									permitted = false;
								}

							}

							if (permitted)
							{
								auto_rotate_on.push_back(CURRENT_OBJECT(currentselectedobj));
							}
						}
						else
						{
							for (size_t i = 0; i < auto_rotate_on.size(); i++)
							{
								if (auto_rotate_on.at(i) == CURRENT_OBJECT(currentselectedobj))
								{
									auto_rotate_on.erase(auto_rotate_on.begin() + i);
								}

							}
						}

						ImGui::SliderFloat("Rotate object", &data.rotationamount, 0.0f, 360.0f);
						ImGui::SliderFloat("Translate object(x axis)", &data.moveamount.x, 0.0f, data.maxmove.x);
						ImGui::SliderFloat("Translate object(y axis)", &data.moveamount.y, 0.0f, data.maxmove.y);
						ImGui::SliderFloat("Translate object(z axis)", &data.moveamount.z, 0.0f, data.maxmove.z);
						ImGui::SliderFloat("Scale object", &data.scaleamount, 0.1f, data.maxscale);
						ImGui::ColorEdit4("Background color", (float*)&std::get<5>(chosen_color_sheme));
						ImGui::Text("Selected Object: %d", currentselectedobj);

						ImGui::EndChildFrame();

					}



					if (ImGui::CollapsingHeader("Input Manuel", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed))
					{
						ImGui::BeginChildFrame(16, ImVec2(ChildMenuSize.x * 0.98f, ChildMenuSize.y * 0.30f), ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);


						ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
						ImGui::Text("Rotate the camera: Middle mouse button");
						ImGui::Text("Move the camera: W,A,S,D keys");
						ImGui::Text("Faster browsing: Left shift key");
						ImGui::Text("Go up and down: Space and right ctrl keys");
						ImGui::Text("Delete object: Delete key");
						ImGui::Text("Duplicate object: Left ctrl + D keys");

						ImGui::EndChildFrame();
					}



					if (ImGui::CollapsingHeader("Material Settings", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed))
					{
						ImGui::BeginChildFrame(10, ImVec2(ChildMenuSize.x * 0.98f, ChildMenuSize.y * 0.80f), ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);

						float AlbedoColor[3] = { data.albedo.x ,data.albedo.y , data.albedo.z };
						ImGui::ColorEdit3("Albedo", AlbedoColor);
						data.albedo({ AlbedoColor[0], AlbedoColor[1], AlbedoColor[2] });

						ImGui::SliderFloat("Metallic", &data.metallic, 0.0f, 1.0f);
						ImGui::SliderFloat("roughness", &data.roughness, 0.0f, 1.0f);
						ImGui::SliderFloat("ao", &data.ao, 0.0f, 1.0f);


						static bool allow_import_texture = false;
						nfdchar_t* texture_path = nullptr;
						int Map_count = 4;

						ImVec4 bgColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

						ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
						const char* button_info = nullptr;

						int map_type = NULL;
						std::string map_type_string;
						static std::vector<int> button_number;
						static bool button_exists = false;




						for (size_t i = 0; i < scene.models.at(currentselectedobj - 2)->meshes.size(); i++)
						{

							std::vector<int> image_import_buttons;

							for (size_t s = 0; s < Map_count; s++)
							{

								int button_type = NULL;


								if (s % Map_count == 0)
								{
									button_info = "Diffuse Map";
									image_import_buttons.push_back(DIFFUSE_TEXTURE);
									button_type = DIFFUSE_TEXTURE;
									map_type_string = "texture_diffuse";
								}
								else if (s % Map_count == 1)
								{
									button_info = "Specular Map";
									image_import_buttons.push_back(SPECULAR_TEXTURE);
									button_type = SPECULAR_TEXTURE;
									map_type_string = "texture_specular";

								}
								else if (s % Map_count == 2)
								{
									button_info = "Normal Map";
									image_import_buttons.push_back(NORMAL_TEXTURE);
									button_type = NORMAL_TEXTURE;
									map_type_string = "texture_normal";

								}
								else if (s % Map_count == 3)
								{
									button_info = "Metalic Map";
									image_import_buttons.push_back(METALIC_TEXTURE);
									button_type = METALIC_TEXTURE;
									map_type_string = "texture_metalic";

								}

								uint button_image_id = NULL;
								bool image_button_enabled = false;

								button_info = (scene.models.at(currentselectedobj - 2)->meshes.at(i).meshname + " " + std::string(button_info)).c_str();

								for (size_t v = 0; v < scene.models.at(currentselectedobj - 2)->meshes.at(i).textures.size(); v++)
								{
									if (std::strcmp(scene.models.at(currentselectedobj - 2)->meshes.at(i).textures.at(v).type.data(), map_type_string.data()) == 0)
									{
										button_image_id = scene.models.at(currentselectedobj - 2)->meshes.at(i).textures.at(v).id;
										image_button_enabled = true;
									}

								}


								//std::cout << "BUTTON TYPE: " << map_type_string << "\n";
								//std::cout << "image_button_enabled: " << image_button_enabled << "\n";



								if (image_button_enabled)
								{
									if (ImGui::ImageButton((void*)(intptr_t)button_image_id, ImVec2(200, 200)))
									{
										button_exists = true;
									}
								}
								else
								{
									if (ImGui::Button(button_info, ImVec2(200, 200)))
									{
										button_exists = true;
										std::cout << "CLICKED ON BUTTON: " << i + 1 << " " << s + 1 << "\n";
									}


								}

								if (button_exists)
								{

									nfdfilteritem_t filterItem[3] = { { "Image files", "PNG,JPG" }, { "PNG", "PNG" } , {"JPG","JPG"} };
									nfdresult_t result = NFD_OpenDialog(&texture_path, filterItem, 3, NULL);

									if (result == NFD_OKAY)
									{
										puts("Success!");
										puts(texture_path);
										allow_import_texture = true;

									}
									else if (result == NFD_CANCEL)
									{
										puts("User pressed cancel.");

									}
									else
									{
										printf("Error: %s\n", NFD_GetError());
									}

									button_exists = false;
								}


								if (allow_import_texture)
								{

									allow_import_texture = false;

									if (texture_path != nullptr)
									{

										Textures newtexture(texture_path, s, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, button_type, map_type_string);

										Texture texture;
										texture.id = *newtexture.GetTexture();
										texture.type = newtexture.GetTexturePbrType_str();
										texture.path = newtexture.GetPathData();

										if (scene.models.at(currentselectedobj - 2)->meshes[i].textures.size() >= s + 1)
										{
											if (std::strcmp(scene.models.at(currentselectedobj - 2)->meshes.at(i).textures.at(s).type.data(), texture.type.data()) == 0)
											{
												Texture temp = scene.models.at(currentselectedobj - 2)->meshes[i].textures.at(s);
												scene.models.at(currentselectedobj - 2)->meshes[i].textures.at(s) = texture;
												scene.models.at(currentselectedobj - 2)->textures_loaded.at(s) = texture;
												glDeleteTextures(1, &temp.id);
											}
											else
											{
												scene.models.at(currentselectedobj - 2)->meshes[i].textures.push_back(texture);
												scene.models.at(currentselectedobj - 2)->textures_loaded.push_back(texture);

											}


										}
										else
										{

											scene.models.at(currentselectedobj - 2)->meshes[i].textures.push_back(texture);
											scene.models.at(currentselectedobj - 2)->textures_loaded.push_back(texture);


										}

										//std::cout << "SIZE OF TEXTURES: " << scene.models.at(currentselectedobj - 2)->meshes[i].textures.size() << "\n";
									}


								}





							}

						}

						ImGui::PopStyleColor();

						//}
						ImGui::EndChildFrame();

					}
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();

					ImGui::EndChildFrame();
				}
				// }

			}
			else if (scene.CURRENT_LIGHT(currentselectedlight) >= NULL)
			{

				ImGui::BeginGroup();

				static float f = 0.0f;
				static int counter = 0;
				static bool importmodel = false;

				ImGui::SetCursorPos(ChildMenuPos);

				//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(ImVec4(45 / 255.0, 55 / 255.0, 71 / 255.0, 28 / 255.0)));
				//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.MenuBackgroundColor));
				//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.ChildMenuColor));

				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.MidMenuColor));
				ImGui::BeginChildFrame(66, ChildMenuSize, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::PopStyleColor();
				//ImGui::PushStyleColor(ImGuiCol_Header | , ImGui::ColorConvertFloat4ToU32(ImVec4(80 / 255.0, 40 / 255.0, 250 / 255.0, 98 / 255.0)));
				//ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Set the background color of the header
				//ImGui::PushStyleVar(Imguistylevarbackground, ImVec2(0, 0)); // Set the background color of what's under the header to be the same as the window
				//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(ImVec4(72 / 255.0, 89 / 255.0, 115 / 255.0, 45 / 255.0)));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.ChildMenuColor));

				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 40.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 3.0f);

				if (ImGui::CollapsingHeader("Light General Settings", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed))
				{
					ImGui::BeginChildFrame(33, ImVec2(ChildMenuSize.x * 0.98f, ChildMenuSize.y * 0.50f), ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);

					//ImGui::SetCursorPos(ImVec2(10, 40));
					ImGui::ColorEdit3("Background color", (float*)&data.clear_color); // Edit 3 floats representing a color
					//ImGui::SetCursorPos(ImVec2(10, 70));
					ImGui::Text("Selected Object: %d", currentselectedobj);
					//ImGui::SetCursorPos(ImVec2(10, 100));
					ImGui::Text("Current existing object count: %d", scene.GetModelCount());
					//ImGui::SetCursorPos(ImVec2(10, 130));
					ImGui::EndChildFrame();
				}

				if (ImGui::CollapsingHeader("Input manuel", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed))
				{
					ImGui::BeginChildFrame(7, ImVec2(ChildMenuSize.x * 0.98f, ChildMenuSize.y * 0.40f), ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);

					//ImGui::SetCursorPos(ImVec2(10, 160));
					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
					//ImGui::SetCursorPos(ImVec2(10, 190));
					ImGui::Text("Rotate the camera: Middle mouse button");
					//ImGui::SetCursorPos(ImVec2(10, 205));
					ImGui::Text("Move the camera: W,A,S,D keys");
					//ImGui::SetCursorPos(ImVec2(10, 220));
					ImGui::Text("Faster browsing: Left shift key");
					//ImGui::SetCursorPos(ImVec2(10, 235));
					ImGui::Text("Go up and down: Space and right ctrl keys");
					//ImGui::SetCursorPos(ImVec2(10, 250));
					ImGui::Text("Delete object: Delete key");
					//ImGui::SetCursorPos(ImVec2(10, 265));
					ImGui::Text("Duplicate object: Left ctrl + D keys");
					ImGui::EndChildFrame();
				}

				if (ImGui::CollapsingHeader("Light Settings", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed))
				{
					ImGui::BeginChildFrame(7, ImVec2(ChildMenuSize.x * 0.98f, ChildMenuSize.y * 0.80f), ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);

					if (ImGui::SliderFloat("Light Intensity", &scene.LightIntensities[scene.CURRENT_LIGHT(currentselectedlight)], 0.0f, 20.0f))
					{
						UseShaderProgram(import_shader);
						GLuint lightIntensities = glGetUniformLocation(import_shader, "lightIntensities");
						glUniform1fv(lightIntensities, MAX_LIGHT_COUNT, &scene.LightIntensities[0]);
						UseShaderProgram(0);
					};

					glm::vec4* c = &scene.LightColors[scene.CURRENT_LIGHT(currentselectedlight)];
					float* color[4] = { &c->x ,&c->y , &c->z,&c->w };

					if (ImGui::ColorEdit4("Light Color", *color))
					{
						UseShaderProgram(import_shader);
						GLuint lightcolors = glGetUniformLocation(import_shader, "lightColors");
						scene.lights[scene.CURRENT_LIGHT(currentselectedlight)]->light_color = *c;
						glUniform4fv(lightcolors, MAX_LIGHT_COUNT, &scene.LightColors[0][0]);
						UseShaderProgram(0);
					};

					ImGui::EndChildFrame();
				}

				ImGui::PopStyleColor();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();


				ImGui::EndChildFrame();

				ImGui::PopStyleColor();

				ImGui::EndGroup();


			}
			else
			{
				ImGui::BeginGroup();
				static float f = 0.0f;
				static int counter = 0;
				static bool importmodel = false;

				ImGui::SetCursorPos(ChildMenuPos);
				//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(ImVec4(45 / 255.0, 55 / 255.0, 71 / 255.0, 28 / 255.0)));
				//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.ChildMenuColor));

				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.MidMenuColor));
				ImGui::BeginChildFrame(66, ChildMenuSize, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::PopStyleColor();
				//ImGui::PushStyleColor(ImGuiCol_Header | , ImGui::ColorConvertFloat4ToU32(ImVec4(80 / 255.0, 40 / 255.0, 250 / 255.0, 98 / 255.0)));
				//ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Set the background color of the header
				//ImGui::PushStyleVar(Imguistylevarbackground, ImVec2(0, 0)); // Set the background color of what's under the header to be the same as the window
				//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(ImVec4(72 / 255.0, 89 / 255.0, 115 / 255.0, 45 / 255.0)));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.ChildMenuColor));
				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 40.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 3.0f);

				if (ImGui::CollapsingHeader("General Settings", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed))
				{
					ImGui::BeginChildFrame(33, ImVec2(ChildMenuSize.x * 0.98f, ChildMenuSize.y * 0.50f), ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);

					ImGui::Text("Enable Skybox:");
					ImGui::SameLine();
					ImGui::Checkbox("##EnableSkybox", &data.render_cube_map);


					static bool ImportHDRI = false;
					if (ImGui::Button("Import HDRI", ImVec2(100, 40)) && !ImportHDRI)
					{
						nfdfilteritem_t filterItem[3] = { { "High Dynamic Range Image Files", "hdr,exr" }, { "hdr", "hdr" } , {"exr","exr"} };
						nfdresult_t ResultHDRI = NFD_OpenDialog(&data.outPath, filterItem, 3, NULL);
						if (ResultHDRI == NFD_OKAY)
						{
							ImportHDRI = true;
							puts("Success!");
							puts(data.outPath);
						}
						else if (ResultHDRI == NFD_CANCEL)
						{
							puts("User pressed cancel.");
							data.imported = true;
							importmodel = false;
						}
						else
						{
							printf("Error: %s\n", NFD_GetError());
						}
					}

					if (ImportHDRI)
					{
						int HDRIstate = ImportCubeMap(data.outPath, Cubemap, HDRItoCubeMapShader, ConvolutateCubeMapShader, PrefilterHDRIShader, data, logs);
						if (HDRIstate == HDRI_COMPLETE)
						{
							data.HDRIpath = data.outPath;
						}
						ImportHDRI = false;
					}



					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::Text("Show light meshes:");
					ImGui::SameLine();
					ImGui::Checkbox("##Showlightmeshes", &data.renderlights);

					ImGui::Spacing();
					ImGui::Text("Camera FOV:");
					ImGui::SameLine();
					ImGui::SliderFloat("##CameraFOV", &data.CameraFOV, 0.0f, 360.0f);
					ImGui::Spacing();

					ImGui::Text("Enable SSAO:");
					ImGui::SameLine();
					ImGui::Checkbox("##EnableSSAO", &data.EnableSSAO);
					if (data.EnableSSAO)
					{
						ImGui::SliderFloat("SSAO radius", &data.SSAOradius, 0.0f, 10.0f);
						ImGui::SliderFloat("SSAO bias", &data.SSAObias, 0.0f, 0.1f);
						ImGui::SliderInt("SSAO Kernel Size", &data.SSAOkernelSize, 0, 128);
					}

					ImGui::Checkbox(" Enable Shadows", &data.RenderShadows);
					if (data.RenderShadows)
					{
						ImGui::SliderInt("Shadow casting light count", &data.ShadowCastingLightCount, 0, MAX_LIGHT_COUNT);
					}

					ImGui::Spacing();
					ImVec2 p = ImGui::GetCursorScreenPos();
					ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (current_win_size.x * 0.01f), p.y), ImVec2(p.x + (current_win_size.x * 0.9f), p.y), ImGui::GetColorU32(current_color_sheme.MenuBackgroundColor), 2.0f);
					ImGui::Spacing();
					ImGui::Checkbox("Enable DOF", &data.DOFenabled);
					if (data.DOFenabled)
					{
						ImGui::SliderFloat("DOF intensity", &data.DOFintensity, 0.0f, 10.0f);
						ImGui::SliderFloat("DOF far distance", &data.DOFfarDistance, 0.0f, 1.0f);
					}

					ImGui::Checkbox("Enable Fog", &data.FogEnabled);
					if (data.FogEnabled)
					{
						ImGui::SliderFloat("Fog intensity", &data.FogIntensity, 0.0f, 100.0f);
						ImGui::ColorEdit4("Fog color", (float*)&data.FogColor);
					}

					ImGui::ColorEdit4("Background color", (float*)&std::get<5>(chosen_color_sheme));

					ImGui::Spacing();

					ImGui::Text("Selected Object: %d", currentselectedobj);
					ImGui::Text("Current existing object count: %d", scene.GetModelCount());

					//ImGui::SliderInt("Active Camera", &ActiveCameraID, 0, Cameras.size() - 1);
					ImGui::EndChildFrame();
				}
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();


				//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(ImVec4(72 / 255.0, 89 / 255.0, 115 / 255.0, 45 / 255.0)));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(current_color_sheme.ChildMenuColor));
				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 40.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 3.0f);

				if (ImGui::CollapsingHeader("Input manuel", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Framed))
				{
					ImGui::BeginChildFrame(7, ImVec2(ChildMenuSize.x * 0.98f, ChildMenuSize.y * 0.40f), ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize);

					//ImGui::SetCursorPos(ImVec2(10, 160));
					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
					//ImGui::SetCursorPos(ImVec2(10, 190));
					ImGui::Text("Rotate the camera: Middle mouse button");
					//ImGui::SetCursorPos(ImVec2(10, 205));
					ImGui::Text("Move the camera: W,A,S,D keys");
					//ImGui::SetCursorPos(ImVec2(10, 220));
					ImGui::Text("Faster browsing: Left shift key");
					//ImGui::SetCursorPos(ImVec2(10, 235));
					ImGui::Text("Go up and down: Space and right ctrl keys");
					//ImGui::SetCursorPos(ImVec2(10, 250));
					ImGui::Text("Delete object: Delete key");
					//ImGui::SetCursorPos(ImVec2(10, 265));
					ImGui::Text("Duplicate object: Left ctrl + D keys");

					ImGui::EndChildFrame();

				}
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImVec2 ButtonSize(std::min(current_win_size.x * 0.3f, 130.0f), std::min(current_win_size.x * 0.2f, 65.0f));

				if (!importmodel)
				{

					ImGui::Text("Import 3D model: ");
					ImGui::SameLine();

					if (ImGui::Button("Import Model", ButtonSize))
					{
						importmodel = true;
						data.imported = false;

						nfdfilteritem_t filterItem[3] = { { "3D object files", "obj,fbx" }, { "obj", "obj" } , {"fbx","fbx"} };
						nfdresult_t result;
						result = NFD_OpenDialog(&data.outPath, filterItem, 3, NULL);
						if (result == NFD_OKAY)
						{
							puts("Success!");
							puts(data.outPath);
						}
						else if (result == NFD_CANCEL)
						{
							puts("User pressed cancel.");
							data.imported = true;
							importmodel = false;
						}
						else
						{
							printf("Error: %s\n", NFD_GetError());
						}
					}
				}

				if (importmodel)
				{

					if (data.outPath != nullptr)
					{
						std::string path(data.outPath);

						for (size_t i = 0; i < path.size(); i++)
						{
							if (path.at(i) == '\\')
							{
								path.at(i) = '/';
							}
						}

						std::cout << "NEW PATH: " << path << "\n";

						std::string temp(path);
						scene.ImportModel(temp, import_shader);
						scene.handlelights(import_shader);

						UseShaderProgram(import_shader);

						//glUniformMatrix4fv(glGetUniformLocation(defaultshader.GetID(), "model"), 1, GL_FALSE, glm::value_ptr(pyramidmodel));
						glUniform4f(glGetUniformLocation(import_shader, "lightColor1"), lightcolor.x, lightcolor.y, lightcolor.z, lightcolor.w);
						glUniform3f(glGetUniformLocation(import_shader, "lightpos1"), lightpos.x, lightpos.y, lightpos.z);


						scene.GetModel(scene.GetModelCount() - 1)->transformation.Scale(glm::vec3(0.05f, 0.05f, 0.05f));

						std::string logtemp = "A new object is imported!";

						logs.push_back(logtemp);

						data.imported = true;
						importmodel = false;
					}


				}


				if (!data.takesreenshot)
				{

					//ImGui::SetCursorPos(ImVec2(10, 360));
					ImGui::Text("");
					ImGui::Text("Save screenshot: ");

					//ImGui::SetCursorPos(ImVec2(130, 350));
					ImGui::SameLine();

					if (ImGui::Button("Save Screenshot", ButtonSize))
					{
						data.takesreenshot = true;
					}

				}
				if (data.takesreenshot)
				{
					nfdfilteritem_t filterItem[2] = { { "Image Files", "png" }, { "png", "png" } };
					nfdresult_t result = NFD_SaveDialog(&data.screenshotPath, filterItem, 2, NULL, NULL);
					if (result == NFD_OKAY)
					{
						puts("Success!");
						puts(data.screenshotPath);
					}
					else if (result == NFD_CANCEL)
					{
						puts("User pressed cancel.");
						data.takesreenshot = false;
					}
					else
					{
						printf("Error: %s\n", NFD_GetError());
					}

					if (data.screenshotPath != nullptr)
					{

						std::string path(data.screenshotPath);

						for (size_t i = 0; i < path.size(); i++)
						{
							if (path.at(i) == '\\')
							{
								path.at(i) = '/';
							}
						}

						data.screenshotPathstr = path;
					}

				}

				ImGui::EndChildFrame();
				ImGui::PopStyleColor();
				ImGui::EndGroup();

			}
		}

		current_win_size({ (int)ImGui::GetWindowSize().x,current_viewport_size.y });

	}

}



