#pragma once
#include "glew.h"
#include "nativefiledialog-extended-master/src/include/nfd.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_impl_glfw.h"
#include "VectorMath.h"
#include "Camera.h"
#define MAX_LIGHT_COUNT 20

typedef std::vector<std::string> LogArray;
extern LogArray logs;
extern Vec2<int> OverAllWindowSize;
extern bool WindowWasResized;

namespace DATA
{
	struct ObjectSelectionState
	{
		int SelectedObject = NULL;
		int SelectedLight = NULL;
		int SelectedGizmo = NULL;
	};

	struct SaveFileData
	{
		int ViewportTheme = -1;
		int HDRIsize = -1;
		int CameraLayout = -1;

		std::vector<std::pair<std::string, int>> RecentProjects;

		bool empty()
		{
			return ViewportTheme == -1 && HDRIsize == -1 && CameraLayout == -1;
		}

	};

	struct UIdataPack
	{
		bool renderlights = false;
		ImVec4 clear_color = ImVec4(0.041f, 0.041f, 0.044f, 0.00f);

		bool autorotate = false;
		float rotationamount = NULL;
		float keepoldrotation = NULL;
		Vec3<float> moveamount = { NULL,NULL,NULL };
		Vec3<float> maxmove = { 20.0f,20.0f,20.0f };
		Vec3<float> newtreshold = { 20.0f,20.0f,20.0f };
		float scaleamount = 1.0f;
		float maxscale = 2.0f;

		GLuint CubeMapSize = 1024;

		bool logbutton = false;
		bool propertiesbutton = false;
		bool Outlinerbutton = false;

		nfdchar_t* outPath = nullptr;
		nfdchar_t* screenshotPath = nullptr;
		std::string screenshotPathstr;
		bool takesreenshot = false;


		char filepath[200];
		bool imported = false;

		bool enablehighlight = false;

		bool EnableSSAO = false;
		float SSAObias = 0.025;
		int SSAOkernelSize = 64;
		float SSAOradius = 0.5f;

		float degree = NULL;

		bool render_cube_map = false;
		std::string HDRIpath;

		Vec3<float> albedo = { 1.0f,1.0f,1.0f };
		float metallic = 0.0f;
		float roughness = 0.5f;
		float ao = 0.1f;

		float LightIntensity = 1.0f;

		bool RenderGrid = true;
		bool SplashScreenEnabled = true;

		SaveFileData saveFileData;

		int cameraLayout = CAMERA_LAYOUT_INDUSTRY_STANDARD;
		float CameraFOV = 45.0f;

		bool IsPreferencesFileEmpty;
		bool RenderShadows = false;
		int ShadowCastingLightCount = MAX_LIGHT_COUNT;

		GLuint ConvDiffCubeMap;
		GLuint PrefilteredEnvMap;
		GLuint brdfLUT;

		std::string cameraLayOutNameStr;

		bool DOFenabled = false;
		float DOFintensity = 1.0f;
		float DOFfarDistance = 0.09f;

		bool FogEnabled = false;
		float FogIntensity = 1.0f;
		ImVec4 FogColor = clear_color;
	};
}