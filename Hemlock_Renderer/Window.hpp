#pragma once
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_internal.h"
#include <glew.h>
#include <glfw3.h>
#include "VectorMath.h"
#include "Log.h"
#include "Scene.h"

typedef std::tuple<ImVec4, ImVec4, ImVec4, ImVec4, ImVec4, ImVec4> color_sheme_t;
// Dark theme color values
#define DARK_THEME color_sheme_t(ImVec4(0.1f, 0.1f, 0.1f, 1.0f), ImVec4(0.2f, 0.2f, 0.2f, 1.0f), ImVec4(0.25f, 0.25f, 0.25f, 1.0f), ImVec4(0.4f, 0.4f, 0.4f, 1.0f), ImVec4(0.8f, 0.8f, 0.8f, 1.0f),ImVec4(0.041f, 0.041f, 0.044f, 0.00f))
#define LIGHT_THEME color_sheme_t(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ImVec4(0.7f, 0.7f, 0.7f, 1.0f), ImVec4(0.6f, 0.6f, 0.6f, 1.0f), ImVec4(0.2f, 0.2f, 0.2f, 1.0f),ImVec4(0.5f, 0.5f, 0.5f, 1.0f))
#define BLUE_THEME color_sheme_t(ImVec4(0.1f, 0.1f, 0.3f, 1.0f), ImVec4(0.2f, 0.2f, 0.6f, 1.0f), ImVec4(0.25f, 0.25f, 0.3f, 1.0f), ImVec4(0.4f, 0.4f, 0.4f, 1.0f), ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ImVec4(0.041f, 0.041f, 0.044f, 0.00f))
#define CUSTOM_THEME color_sheme_t(ImVec4(0.3f, 0.3f, 0.3f, 1.0f), ImVec4(0.4f, 0.4f, 0.4f, 1.0f), ImVec4(0.35f, 0.35f, 0.35f, 1.0f),  ImVec4(0.7f, 0.7f, 0.7f, 1.0f), ImVec4(0.9f, 0.9f, 0.9f, 1.0f), ImVec4(0.1f, 0.1f, 0.1f, 1.0f))
#define PINK_THEME color_sheme_t(ImVec4(0.3f, 0.1f, 0.1f, 1.0f), ImVec4(0.6f, 0.2f, 0.2f, 1.0f), ImVec4(0.3f, 0.25f, 0.25f, 1.0f), ImVec4(0.4f, 0.4f, 0.4f, 1.0f), ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ImVec4(0.044f, 0.041f, 0.041f, 0.00f))
#define CYAN_THEME color_sheme_t(ImVec4(0.0f, 0.2f, 0.2f, 1.0f), ImVec4(0.0f, 0.4f, 0.4f, 1.0f), ImVec4(0.0f, 0.5f, 0.5f, 1.0f), ImVec4(0.0f, 0.8f, 0.8f, 1.0f), ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f))
#define ORANGE_THEME color_sheme_t(ImVec4(0.8f, 0.4f, 0.1f, 1.0f), ImVec4(1.0f, 0.5f, 0.2f, 1.0f), ImVec4(1.0f, 0.6f, 0.3f, 1.0f), ImVec4(1.0f, 0.7f, 0.4f, 1.0f), ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f))
#define GREEN_THEME color_sheme_t(ImVec4(0.1f, 0.3f, 0.1f, 1.0f), ImVec4(0.2f, 0.5f, 0.2f, 1.0f), ImVec4(0.25f, 0.6f, 0.25f, 1.0f), ImVec4(0.4f, 0.7f, 0.4f, 1.0f), ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f))
#define NORDIC_THEME color_sheme_t(ImVec4(0.05f, 0.05f, 0.05f, 1.0f), ImVec4(0.16f, 0.23f, 0.24f, 1.0f), ImVec4(0.20f, 0.29f, 0.30f, 1.0f), ImVec4(0.26f, 0.35f, 0.36f, 1.0f), ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f))
#define GITHUB_STYLE_THEME color_sheme_t(ImVec4(0.13f, 0.17f, 0.23f, 1.0f), ImVec4(0.18f, 0.22f, 0.28f, 1.0f), ImVec4(0.22f, 0.26f, 0.32f, 1.0f), ImVec4(0.28f, 0.32f, 0.38f, 1.0f), ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f))
#define INTELLIJ_STYLE_THEME color_sheme_t(ImVec4(0.95f, 0.95f, 0.95f, 1.0f), ImVec4(0.25f, 0.27f, 0.30f, 1.0f), ImVec4(0.2f, 0.22f, 0.25f, 1.0f), ImVec4(0.12f, 0.14f, 0.17f, 1.0f), ImVec4(0.38f, 0.40f, 0.43f, 1.0f), ImVec4(0.6f, 0.63f, 0.66f, 1.0f))
#define CHATGPT_LIGHT_THEME color_sheme_t(ImVec4(0.95f, 0.95f, 0.95f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(0.9f, 0.9f, 0.9f, 1.0f), ImVec4(0.8f, 0.8f, 0.8f, 1.0f), ImVec4(0.2f, 0.2f, 0.2f, 1.0f), ImVec4(0.1f, 0.1f, 0.1f, 1.0f))
#define SUNSET_THEME color_sheme_t(ImVec4(0.95f, 0.85f, 0.80f, 1.0f), ImVec4(0.54f, 0.29f, 0.31f, 1.0f), ImVec4(0.45f, 0.22f, 0.25f, 1.0f), ImVec4(0.35f, 0.16f, 0.20f, 1.0f), ImVec4(0.68f, 0.41f, 0.31f, 1.0f), ImVec4(0.89f, 0.56f, 0.41f, 1.0f))
#define OCEANIC_THEME color_sheme_t(ImVec4(0.15f, 0.20f, 0.25f, 1.0f), ImVec4(0.23f, 0.35f, 0.42f, 1.0f), ImVec4(0.19f, 0.28f, 0.35f, 1.0f), ImVec4(0.16f, 0.22f, 0.28f, 1.0f), ImVec4(0.37f, 0.54f, 0.63f, 1.0f), ImVec4(0.48f, 0.71f, 0.84f, 1.0f))
#define VINTAGE_THEME color_sheme_t(ImVec4(0.95f, 0.92f, 0.82f, 1.0f), ImVec4(0.51f, 0.39f, 0.31f, 1.0f), ImVec4(0.42f, 0.32f, 0.25f, 1.0f), ImVec4(0.35f, 0.26f, 0.20f, 1.0f), ImVec4(0.66f, 0.50f, 0.40f, 1.0f), ImVec4(0.82f, 0.64f, 0.51f, 1.0f))
#define MINTY_FRESH_THEME color_sheme_t(ImVec4(0.87f, 0.98f, 0.93f, 1.0f), ImVec4(0.21f, 0.55f, 0.40f, 1.0f), ImVec4(0.17f, 0.46f, 0.33f, 1.0f), ImVec4(0.13f, 0.36f, 0.26f, 1.0f), ImVec4(0.31f, 0.73f, 0.56f, 1.0f), ImVec4(0.48f, 0.85f, 0.65f, 1.0f))
#define ROYAL_PURPLE_THEME color_sheme_t(ImVec4(0.85f, 0.80f, 0.95f, 1.0f), ImVec4(0.37f, 0.20f, 0.54f, 1.0f), ImVec4(0.31f, 0.22f, 0.45f, 1.0f), ImVec4(0.25f, 0.16f, 0.35f, 1.0f), ImVec4(0.47f, 0.41f, 0.68f, 1.0f), ImVec4(0.61f, 0.56f, 0.89f, 1.0f))

#define DARK_THEME_ID 0x80
#define LIGHT_THEME_ID 0x81
#define BLUE_THEME_ID 0x82 
#define CUSTOM_THEME_ID 0x83 
#define PINK_THEME_ID 0x84
#define CYAN_THEME_ID 0x85 
#define ORANGE_THEME_ID 0x86 
#define GREEN_THEME_ID 0x87
#define NORDIC_THEME_ID 0x88
#define GITHUB_STYLE_THEME_ID 0x89
#define INTELLIJ_STYLE_THEME_ID 0x90
#define CHATGPT_LIGHT_THEME_ID 0x91
#define SUNSET_THEME_ID 0x92 
#define OCEANIC_THEME_ID 0x93 
#define VINTAGE_THEME_ID 0x94 
#define MINTY_FRESH_THEME_ID 0x95 
#define ROYAL_PURPLE_THEME_ID 0x96

namespace UI
{
	struct UIcolorShemePack
	{
		ImVec4 MenuBackgroundColor;
		ImVec4 ChildMenuColor;
		ImVec4 MidMenuColor;
		ImVec4 BorderColor;
		ImVec4 TextColor;
		ImVec4 ViewportBackgroundColor;

		void SetMenuBackgroundColor(ImVec4 mbgcolor)
		{
			MenuBackgroundColor = mbgcolor;
		}

		void SetChildMenuColor(ImVec4 cmcolor)
		{
			ChildMenuColor = cmcolor;
		}

		void operator()(const color_sheme_t& color_sh)
		{
			MenuBackgroundColor = std::get<0>(color_sh);
			ChildMenuColor = std::get<1>(color_sh);
			MidMenuColor = std::get<2>(color_sh);
			BorderColor = std::get<3>(color_sh);
			TextColor = std::get<4>(color_sh);
			ViewportBackgroundColor = std::get<5>(color_sh);
		}

	};
}

namespace WINDOWMANAGER
{

#define WM_WINDOW_OUTLINER 0x1004
#define WM_WINDOW_LOGS 0x1005
#define WM_WINDOW_RENDERSETTINGS 0x1006

	class Window
	{
	public:

		Window() = default;

		virtual void Update();
		void UpdateLocalWindowSize();
		bool IsOpen() { return WindowOpen; };
		int GetWindowType() { return WindowType; };
		unsigned int GetWindowID() { return this->WindowID; };
		std::string GetWindowName() { return this->WindowName; };
		void SetWindowName(std::string&& WindowName);

	protected:
		Vec2<int> WindowSize;
		Vec2<int> WindowPosition;
		ImRect WindowRect;

		bool WindowOpen = true;
		Vec2<int> GlobalWindowSize;
		UI::UIcolorShemePack *GlobalColorSheme;
		int WindowType;
		unsigned int WindowID;
		std::string WindowName;
	};

	class OutlinerWindow : public Window
	{
	public:

		OutlinerWindow(scene& scene, DATA::ObjectSelectionState& SelectionState, UI::UIcolorShemePack &GlobalColorSheme, Shader& ImportShader);
		void Update() override;

	private:
		scene* GlobalScene;
		DATA::ObjectSelectionState* GlobalSelectionState;
		GLuint ImportShader;
	};

	class LogsWindow : public Window
	{
	public:
		LogsWindow(UI::UIcolorShemePack& GlobalColorSheme);
		void Update() override;

	private:

		
	};

	class RenderSettings : public Window
	{
	public:
		RenderSettings();
		void Update() override;
	private:

	};

	extern std::vector<WINDOWMANAGER::Window*> Windows;

	template<class T, class... Args>
	void AddNewWindow(Args&&... args)
	{
		T *NewWindow = new T(std::forward<Args>(args)...);
		Windows.push_back(NewWindow);
	} 

	void WindowGarbageCollector();
	void DisposeWindows();
	void UpdateWindows();
}




