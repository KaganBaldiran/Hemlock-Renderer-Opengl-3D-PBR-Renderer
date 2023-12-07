#pragma once
#include <glew.h>
#include <glfw3.h>
#include "../dependencies/nlohmann/json.hpp"
#include "VectorMath.h"
#include "Log.h"
#include <algorithm>
#include "Scene.h"
#include "nativefiledialog-extended-master/src/include/nfd.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_impl_glfw.h"
using json = nlohmann::json;

#define HML_FILE 0x802
#define HML_FILE_PACKED 0x803

namespace SAVEFILE
{
	

	bool ReadSaveFile(const char* fileName, DATA::SaveFileData& data, std::vector<std::string>& logs);
	void WriteSaveFile(const char* fileName, DATA::SaveFileData& data, std::vector<std::string>& logs);
	void WriteHMLfilePacked(const char* fileName, scene& scene, DATA::UIdataPack& data, Camera& camera, int renderPass, std::vector<std::string>& logs);
	void WriteHMLfile(const char* fileName, scene& scene, DATA::UIdataPack& data, Camera& camera, int renderPass, std::vector<std::string>& logs);
	void ReadHMLfile(const char* fileName, scene& scene, GLuint shader, GLuint lightShader, DATA::UIdataPack& data, Camera& camera, int &renderPass, std::vector<std::string>& logs, GLFWwindow* window, CubeMap& cubemap, GLuint& HDRItoCubeMapShader, GLuint& ConvolutateCubeMapShader, GLuint& PrefilterHDRIShader);
	void ReadHMLfilePacked(const char* fileName, scene& scene, GLuint shader, GLuint lightShader, DATA::UIdataPack& data, Camera& camera, int& renderPass, std::vector<std::string>& logs, GLFWwindow* window, CubeMap& cubemap, GLuint& HDRItoCubeMapShader, GLuint& ConvolutateCubeMapShader, GLuint& PrefilterHDRIShader);
}