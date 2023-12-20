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
#define OBJECT_FILE 0x904
#define IMAGE_FILE 0x905
#define HDRI_FILE 0x906
#define CONVERSION_KILOBYTE 0x1002
#define CONVERSION_MEGABYTE 0x1003
#define CONVERSION_GIGABYTE 0x1004

bool StrCmprCaseInsnstv(const char* str1, const char* str2);

template<int ConversionType>
inline float GetFileSize(const char* FilePath)
{
	std::filesystem::path p{ FilePath };
	float FileSize = std::filesystem::file_size(p);
	switch (ConversionType)
	{
	case CONVERSION_KILOBYTE:
		FileSize /= 1024.0f;
		break;
	case CONVERSION_MEGABYTE:
		FileSize /= (float)pow(1024, 2);
		break;
	case CONVERSION_GIGABYTE:
		FileSize /= (float)pow(1024, 3);
		break;
	default:
		break;
	};
	return FileSize;
};

namespace SAVEFILE
{
	int CheckHMLfileType(const char* fileName);
	void ClearScene(scene& scen, DATA::UIdataPack& data, GLuint shader);
	bool ReadSaveFile(const char* fileName, DATA::SaveFileData& data, std::vector<std::string>& logs);
	void WriteSaveFile(const char* fileName, DATA::SaveFileData& data, std::vector<std::string>& logs);
	void WriteHMLfilePacked(const char* fileName, scene& scene, DATA::UIdataPack& data, Camera& camera, int renderPass, std::vector<std::string>& logs);
	void WriteHMLfile(const char* fileName, scene& scene, DATA::UIdataPack& data, Camera& camera, int renderPass, std::vector<std::string>& logs);
	void ReadHMLfile(const char* fileName, scene& scene, GLuint shader, GLuint lightShader, DATA::UIdataPack& data, Camera& camera, int &renderPass, std::vector<std::string>& logs, GLFWwindow* window, CubeMap& cubemap, GLuint& HDRItoCubeMapShader, GLuint& ConvolutateCubeMapShader, GLuint& PrefilterHDRIShader);
	void ReadHMLfilePacked(const char* fileName, scene& scene, GLuint shader, GLuint lightShader, DATA::UIdataPack& data, Camera& camera, int& renderPass, std::vector<std::string>& logs, GLFWwindow* window, CubeMap& cubemap, GLuint& HDRItoCubeMapShader, GLuint& ConvolutateCubeMapShader, GLuint& PrefilterHDRIShader);
    
	class DropDownFileData
	{
	public:
		std::string DropDownFilePath;
		bool DropDownImport = false;
		bool FinalizeImport = false;
		int OverAllFileType = -1;
		float DropDownFileSize = 0.0f;

		std::string ModelToAssignName;
		int ModelToAssignID = -1;
		int MeshToAssignID = -1;
		std::string ImportedTextureUsage;

		GLuint ModelPreview;
		bool MakeModelPreview = false;
		Model* ImportedPreviewModel = nullptr;
	};

}