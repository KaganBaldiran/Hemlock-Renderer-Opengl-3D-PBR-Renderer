#include "SaveFile.h"
#include <filesystem>
#include <fstream>
#include "Texture.h"
#include <iostream>
#include <filesystem>

bool IsFileEmpty(std::ifstream& file) {
	return file.peek() == std::ifstream::traits_type::eof();
}
bool StrCmprCaseInsnstv(const char* str1, const char* str2)
{
	return _stricmp(str1, str2) == 0;
}
int SAVEFILE::CheckHMLfileType(const char* fileName)
{
	std::ifstream File(fileName);
	int Type = -1;
	if (File.is_open())
	{
		if (!IsFileEmpty(File))
		{
			try
			{
				json HMLfile;
				File >> HMLfile;
				File.close();
				const int ReadFileType = HMLfile["FileType"];
				if (ReadFileType == HML_FILE || ReadFileType == HML_FILE_PACKED)
				{
					Type = ReadFileType;
				}
			}
			catch (const std::exception& e)
			{
				LOG_ERR("Exception while reading the HML file :: " << e.what());
				std::string logtemp = "Exception while reading the HML file :: " + std::string(e.what()) + " :: " + std::string(fileName);
				LOG_TO_FILE("Exception_log.txt", logtemp << " :: " << __TIME__ << " :: " << __DATE__);
				File.close();
			}
		}
		
	}
	return Type;
}

void SAVEFILE::ClearScene(scene& scene, DATA::UIdataPack& data, GLuint shader)
{
	for (size_t i = 1; i < scene.models.size(); i++)
	{
		scene.DeleteModel(i);
	}



	DisposeCameras();
	scene.DeleteLights();
}

bool SAVEFILE::ReadSaveFile(const char* fileName, DATA::SaveFileData& data, std::vector<std::string>& logs)
{
	std::ifstream File(fileName);

	if (File.is_open())
	{
		try
		{
			if (!IsFileEmpty(File))
			{
				json SaveFile;
				File >> SaveFile;
				File.close();

				if (!SaveFile.empty())
				{
					data.HDRIsize = SaveFile["HDRIsize"];
					data.ViewportTheme = SaveFile["ViewportTheme"];
					data.CameraLayout = SaveFile["CameraLayout"];

					for (size_t i = 0; i < SaveFile["RecentProjectCount"]; i++)
					{
						std::string projectName("project" + std::to_string(i));

						data.RecentProjects.push_back({ SaveFile["RecentProjects"][projectName]["directory"],
							                            SaveFile["RecentProjects"][projectName]["type"] });
					}

					LOG_INF("Read the preferences!");
				}

			}
			else
			{
				return false;
				LOG_INF("Preferences file is empty!");
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERR("Exception while reading the preferences file :: " << e.what());
			std::string logtemp = "Exception while reading the preferences file :: " + std::string(e.what()) + " :: " + std::string(fileName);
			LOG_TO_FILE("Exception_log.txt", logtemp << " :: " << __TIME__ << " :: " << __DATE__);
			logs.push_back(logtemp);
			File.close();
		}
	}
	else
	{
		return false;
		LOG_ERR("Unable to open the preferences file to read!");
	}

	return true;
}

void SAVEFILE::WriteSaveFile(const char* fileName, DATA::SaveFileData& data, std::vector<std::string>& logs)
{
	if (!data.empty())
	{
		std::ofstream File(fileName);

		if (File.is_open())
		{
			try
			{
				json SaveFile;

				SaveFile["HDRIsize"] = data.HDRIsize;
				SaveFile["ViewportTheme"] = data.ViewportTheme;
				SaveFile["CameraLayout"] = data.CameraLayout;

				SaveFile["RecentProjectCount"] = data.RecentProjects.size();

				for (size_t i = 0; i < data.RecentProjects.size(); i++)
				{
					std::string projectName("project" + std::to_string(i));
					bool allow = true;

					for (size_t y = i + 1; y < data.RecentProjects.size(); y++)
					{
						if (data.RecentProjects.at(i).first == data.RecentProjects.at(y).first)
						{
							allow = false;
							break;
						}
					}
					if (allow)
					{
						SaveFile["RecentProjects"][projectName]["directory"] = data.RecentProjects.at(i).first;
						SaveFile["RecentProjects"][projectName]["type"] = data.RecentProjects.at(i).second;
					}	
				}

				File << SaveFile;

				File.close();
				LOG_INF("Prefences saved!");
			}
			catch (const std::exception& e)
			{
				LOG_ERR("Exception while writing the preferences file :: " << e.what());
				std::string logtemp = "Exception while writing the preferences file :: " + std::string(e.what()) + " :: " + std::string(fileName);
				LOG_TO_FILE("Exception_log.txt", logtemp << " :: " << __TIME__ <<" :: " << __DATE__);
				logs.push_back(logtemp);
				File.close();
			}
		}
		else
		{
			LOG_ERR("Unable to open the preferences file!");
		}
	}

}

void SAVEFILE::WriteHMLfilePacked(const char* fileName, scene& scene, DATA::UIdataPack& data, Camera& camera, int renderPass, std::vector<std::string>& logs)
{

	std::string filePath(fileName);
	int lastSlash;
	std::string DirectoryName(strrchr(fileName, '/'));
	
	lastSlash = DirectoryName.find(".");
	DirectoryName = DirectoryName.substr(1, lastSlash - 1);
	filePath = filePath.substr(0, filePath.size() - 4);

	std::string hmlFileDir(filePath + "/" + DirectoryName + ".hml");

	LOG("DIRECTORY NAME: " << DirectoryName << " " << filePath << " " << hmlFileDir);

	std::string ModelsDirectory(filePath + "/models");
	std::string TexturesDirectory(filePath + "/textures");

	std::filesystem::create_directory(filePath);
	std::filesystem::create_directory(ModelsDirectory);
	std::filesystem::create_directory(TexturesDirectory);

	std::ofstream File(hmlFileDir);

	if (File.is_open())
	{
		try
		{
			json HMLfile;

			HMLfile["modelCount"] = scene.GetModelCount();
			HMLfile["FileType"] = HML_FILE_PACKED;

			for (size_t i = 1; i < scene.GetModelCount(); i++)
			{
				std::string modelID("model" + std::to_string(i));

				HMLfile["models"][modelID]["directory"] = "models" + std::string(strrchr(scene.models[i]->ModelImportPath.c_str(), '/'));
				LOG("MESH NAME: " << "models" + std::string(strrchr(scene.models[i]->ModelImportPath.c_str(), '/')));

				std::filesystem::copy(scene.models[i]->modelpath, ModelsDirectory, std::filesystem::copy_options::recursive);

				for (size_t meshid = 0; meshid < scene.models[i]->meshes.size(); meshid++)
				{
					std::string meshidname("mesh" + std::to_string(meshid));

					HMLfile["models"][modelID][meshidname]["textureCount"] = scene.models[i]->meshes[meshid].textures.size();

					for (size_t textureid = 0; textureid < scene.models[i]->meshes[meshid].textures.size(); textureid++)
					{
						HMLfile["models"][modelID][meshidname]["textures"][textureid]["directory"] = "textures" + std::string(strrchr(scene.models[i]->meshes[meshid].textures[textureid].path.c_str(), '\\'));
						std::filesystem::copy(scene.models[i]->meshes[meshid].textures[textureid].path, TexturesDirectory, std::filesystem::copy_options::recursive);
						LOG("TEXTURE DIRECTORY: " << "textures\\" + std::string(strrchr(scene.models[i]->meshes[meshid].textures[textureid].path.c_str(), '\\')));
						HMLfile["models"][modelID][meshidname]["textures"][textureid]["type"] = scene.models[i]->meshes[meshid].textures[textureid].type;
					}
				}

				HMLfile["models"][modelID]["attributes"]["position"]["x"] = scene.models[i]->transformation.TranslationMatrix[3][0];
				HMLfile["models"][modelID]["attributes"]["position"]["y"] = scene.models[i]->transformation.TranslationMatrix[3][1];
				HMLfile["models"][modelID]["attributes"]["position"]["z"] = scene.models[i]->transformation.TranslationMatrix[3][2];

				HMLfile["models"][modelID]["attributes"]["scale"]["x"] = scene.models[i]->transformation.ScalingMatrix[0][0];
				HMLfile["models"][modelID]["attributes"]["scale"]["y"] = scene.models[i]->transformation.ScalingMatrix[1][1];
				HMLfile["models"][modelID]["attributes"]["scale"]["z"] = scene.models[i]->transformation.ScalingMatrix[2][2];
			}


			HMLfile["lightCount"] = scene.numberoflights;

			for (size_t i = 0; i < scene.numberoflights; i++)
			{
				std::string LightID("light" + std::to_string(i));

				HMLfile["lights"][LightID]["attributes"]["position"]["x"] = scene.LightPositions[i].x;
				HMLfile["lights"][LightID]["attributes"]["position"]["y"] = scene.LightPositions[i].y;
				HMLfile["lights"][LightID]["attributes"]["position"]["z"] = scene.LightPositions[i].z;

				HMLfile["lights"][LightID]["attributes"]["intensity"] = scene.LightIntensities[i];

				HMLfile["lights"][LightID]["attributes"]["color"]["r"] = scene.LightColors[i].x;
				HMLfile["lights"][LightID]["attributes"]["color"]["g"] = scene.LightColors[i].y;
				HMLfile["lights"][LightID]["attributes"]["color"]["b"] = scene.LightColors[i].z;
				HMLfile["lights"][LightID]["attributes"]["color"]["a"] = scene.LightColors[i].w;
			}


			HMLfile["GeneralAttributes"]["SSAO"]["enableSSAO"] = data.EnableSSAO;
			HMLfile["GeneralAttributes"]["SSAO"]["SSAObias"] = data.SSAObias;
			HMLfile["GeneralAttributes"]["SSAO"]["SSAOkernelSize"] = data.SSAOkernelSize;
			HMLfile["GeneralAttributes"]["SSAO"]["SSAOradius"] = data.SSAOradius;

			HMLfile["GeneralAttributes"]["DOF"]["DOFenabled"] = data.DOFenabled;
			HMLfile["GeneralAttributes"]["DOF"]["DOFfarDistance"] = data.DOFfarDistance;
			HMLfile["GeneralAttributes"]["DOF"]["DOFintensity"] = data.DOFintensity;

			HMLfile["GeneralAttributes"]["Fog"]["FogEnabled"] = data.FogEnabled;
			HMLfile["GeneralAttributes"]["Fog"]["FogIntensity"] = data.FogIntensity;

			HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["x"] = data.FogColor.x;
			HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["y"] = data.FogColor.y;
			HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["z"] = data.FogColor.z;

			HMLfile["GeneralAttributes"]["renderGrid"] = data.RenderGrid;

			HMLfile["GeneralAttributes"]["HDRI"]["enableSkybox"] = data.render_cube_map;
			HMLfile["GeneralAttributes"]["HDRI"]["HDRIpath"] = data.HDRIpath;

			HMLfile["GeneralAttributes"]["showLightMeshes"] = data.renderlights;
			HMLfile["GeneralAttributes"]["renderShadows"] = data.RenderShadows;

			HMLfile["ViewportAttributes"]["CameraCount"] = Cameras.size();
			HMLfile["ViewportAttributes"]["ActiveCameraID"] = ActiveCameraID;

			for (size_t i = 0; i < Cameras.size(); i++)
			{
				std::string cameraID = "ID" + std::to_string(i);
				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["x"] = Cameras[i]->Position.x;
				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["y"] = Cameras[i]->Position.y;
				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["z"] = Cameras[i]->Position.z;

				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["x"] = Cameras[i]->Orientation.x;
				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["y"] = Cameras[i]->Orientation.y;
				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["z"] = Cameras[i]->Orientation.z;
			}

			HMLfile["ViewportAttributes"]["cameraFOV"] = data.CameraFOV;
			HMLfile["ViewportAttributes"]["renderPass"] = renderPass;


			File << HMLfile;
			LOG_INF("hml file saved :: " << fileName);
			std::string logtemp = "hml file saved :: " + std::string(fileName);
			logs.push_back(logtemp);
			File.close();
		}
		catch (const std::exception& e)
		{
			LOG_ERR("Exception while writing packed hml file :: " << e.what());
			std::string logtemp = "Exception while writing packed hml file :: " + std::string(e.what()) + " :: " + std::string(fileName);
			LOG_TO_FILE("Exception_log.txt", logtemp << " :: " << __TIME__ << " :: " << __DATE__);
			logs.push_back(logtemp);
			File.close();
		}
	}
	else
	{
		LOG_ERR("Unable to open the file :: " << fileName);
		std::string logtemp = "Unable to open the file :: " + std::string(fileName);
		LOG_TO_FILE("Exception_log.txt", logtemp << " :: " << __TIME__ << " :: " << __DATE__);
		logs.push_back(logtemp);
	}
	
	
}


void SAVEFILE::WriteHMLfile(const char* fileName, scene& scene , DATA::UIdataPack& data,Camera& camera , int renderPass, std::vector<std::string>& logs)
{
	std::ofstream File(fileName);

	if (File.is_open())
	{
		try
		{
			json HMLfile;

			HMLfile["modelCount"] = scene.GetModelCount();
			HMLfile["FileType"] = HML_FILE;

			for (size_t i = 1; i < scene.GetModelCount(); i++)
			{
				std::string modelID("model" + std::to_string(i));
				HMLfile["models"][modelID]["directory"] = scene.models[i]->ModelImportPath;

				for (size_t meshid = 0; meshid < scene.models[i]->meshes.size(); meshid++)
				{
					std::string meshidname("mesh" + std::to_string(meshid));

					HMLfile["models"][modelID][meshidname]["textureCount"] = scene.models[i]->meshes[meshid].textures.size();

					for (size_t textureid = 0; textureid < scene.models[i]->meshes[meshid].textures.size(); textureid++)
					{
						HMLfile["models"][modelID][meshidname]["textures"][textureid]["directory"] = scene.models[i]->meshes[meshid].textures[textureid].path;
						HMLfile["models"][modelID][meshidname]["textures"][textureid]["type"] = scene.models[i]->meshes[meshid].textures[textureid].type;
					}
				}

				for (int row = 0; row < 4; row++) {
					for (int col = 0; col < 4; col++) {
						cout << "Element [" << row << "][" << col << "]: " << scene.models[i]->transformation.GetModelMat4()[row][col] << endl;
					}
				}

				HMLfile["models"][modelID]["attributes"]["position"]["x"] = scene.models[i]->transformation.TranslationMatrix[3][0];
				HMLfile["models"][modelID]["attributes"]["position"]["y"] = scene.models[i]->transformation.TranslationMatrix[3][1];
				HMLfile["models"][modelID]["attributes"]["position"]["z"] = scene.models[i]->transformation.TranslationMatrix[3][2];

				HMLfile["models"][modelID]["attributes"]["scale"]["x"] = scene.models[i]->transformation.ScalingMatrix[0][0];
				HMLfile["models"][modelID]["attributes"]["scale"]["y"] = scene.models[i]->transformation.ScalingMatrix[1][1];
				HMLfile["models"][modelID]["attributes"]["scale"]["z"] = scene.models[i]->transformation.ScalingMatrix[2][2];

			}

			HMLfile["lightCount"] = scene.numberoflights;

			for (size_t i = 0; i < scene.numberoflights; i++)
			{
				std::string LightID("light" + std::to_string(i));

				HMLfile["lights"][LightID]["attributes"]["position"]["x"] = scene.LightPositions[i].x;
				HMLfile["lights"][LightID]["attributes"]["position"]["y"] = scene.LightPositions[i].y;
				HMLfile["lights"][LightID]["attributes"]["position"]["z"] = scene.LightPositions[i].z;

				HMLfile["lights"][LightID]["attributes"]["intensity"] = scene.LightIntensities[i];

				HMLfile["lights"][LightID]["attributes"]["color"]["r"] = scene.LightColors[i].x;
				HMLfile["lights"][LightID]["attributes"]["color"]["g"] = scene.LightColors[i].y;
				HMLfile["lights"][LightID]["attributes"]["color"]["b"] = scene.LightColors[i].z;
				HMLfile["lights"][LightID]["attributes"]["color"]["a"] = scene.LightColors[i].w;
			}


			HMLfile["GeneralAttributes"]["SSAO"]["enableSSAO"] = data.EnableSSAO;
			HMLfile["GeneralAttributes"]["SSAO"]["SSAObias"] = data.SSAObias;
			HMLfile["GeneralAttributes"]["SSAO"]["SSAOkernelSize"] = data.SSAOkernelSize;
			HMLfile["GeneralAttributes"]["SSAO"]["SSAOradius"] = data.SSAOradius;

			HMLfile["GeneralAttributes"]["DOF"]["DOFenabled"] = data.DOFenabled;
			HMLfile["GeneralAttributes"]["DOF"]["DOFfarDistance"] = data.DOFfarDistance;
			HMLfile["GeneralAttributes"]["DOF"]["DOFintensity"] = data.DOFintensity;

			HMLfile["GeneralAttributes"]["Fog"]["FogEnabled"] = data.FogEnabled;
			HMLfile["GeneralAttributes"]["Fog"]["FogIntensity"] = data.FogIntensity;

			HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["x"] = data.FogColor.x;
			HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["y"] = data.FogColor.y;
			HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["z"] = data.FogColor.z;

			HMLfile["GeneralAttributes"]["renderGrid"] = data.RenderGrid;

			HMLfile["GeneralAttributes"]["HDRI"]["enableSkybox"] = data.render_cube_map;
			HMLfile["GeneralAttributes"]["HDRI"]["HDRIpath"] = data.HDRIpath;

			HMLfile["GeneralAttributes"]["showLightMeshes"] = data.renderlights;
			HMLfile["GeneralAttributes"]["renderShadows"] = data.RenderShadows;

			HMLfile["ViewportAttributes"]["CameraCount"] = Cameras.size();
			HMLfile["ViewportAttributes"]["ActiveCameraID"] = ActiveCameraID;

			for (size_t i = 0; i < Cameras.size(); i++)
			{
				std::string cameraID = "ID" + std::to_string(i);
				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["x"] = Cameras[i]->Position.x;
				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["y"] = Cameras[i]->Position.y;
				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["z"] = Cameras[i]->Position.z;

				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["x"] = Cameras[i]->Orientation.x;
				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["y"] = Cameras[i]->Orientation.y;
				HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["z"] = Cameras[i]->Orientation.z;
			}

			HMLfile["ViewportAttributes"]["cameraFOV"] = data.CameraFOV;

			HMLfile["ViewportAttributes"]["renderPass"] = renderPass;


			File << HMLfile;
			LOG_INF("hml file saved :: " << fileName);
			std::string logtemp = "hml file saved :: " + std::string(fileName);
			logs.push_back(logtemp);
			File.close();
		}
		catch (const std::exception& e)
		{
			LOG_ERR("Exception while writing hml file :: " << e.what());
			std::string logtemp = "Exception while writing hml file :: " + std::string(e.what()) + " :: " + std::string(fileName);
			LOG_TO_FILE("Exception_log.txt", logtemp);
			logs.push_back(logtemp);
			File.close();
		}
	}
	else
	{
		LOG_ERR("Unable to open the file :: " << fileName);
		std::string logtemp = "Unable to open the file :: " + std::string(fileName);
		LOG_TO_FILE("Exception_log.txt", logtemp << " :: " << __TIME__ << " :: " << __DATE__);
		logs.push_back(logtemp);
	}

}

void SAVEFILE::ReadHMLfile(const char* fileName, scene& scene , GLuint shader ,GLuint lightShader, DATA::UIdataPack& data, Camera& camera, int& renderPass, std::vector<std::string> &logs , 
	GLFWwindow *window , CubeMap& cubemap, GLuint& HDRItoCubeMapShader , GLuint& ConvolutateCubeMapShader , GLuint& PrefilterHDRIShader)
{
	std::ifstream File(fileName);

	if (File.is_open())
	{
		try
		{
			ClearScene(scene, data, shader);
			json HMLfile;
			File >> HMLfile;

			for (size_t i = 1; i < HMLfile["modelCount"]; i++)
			{
				std::string modelID("model" + std::to_string(i));
				scene.ImportModel(HMLfile["models"][modelID]["directory"], shader);

				for (size_t meshid = 0; meshid < scene.models[i]->meshes.size(); meshid++)
				{
					std::string meshidname("mesh" + std::to_string(meshid));

					for (size_t textureid = 0; textureid < HMLfile["models"][modelID][meshidname]["textureCount"]; textureid++)
					{
						std::string texturePath(HMLfile["models"][modelID][meshidname]["textures"][textureid]["directory"]);
						std::string textureType(HMLfile["models"][modelID][meshidname]["textures"][textureid]["type"]);

						scene.ImportTextureIntoModel(texturePath.c_str(), i, meshid, textureType.c_str());
					}
				}

				glm::vec3 position;
				glm::vec3 scale;

				position.x = HMLfile["models"][modelID]["attributes"]["position"]["x"];
				position.y = HMLfile["models"][modelID]["attributes"]["position"]["y"];
				position.z = HMLfile["models"][modelID]["attributes"]["position"]["z"];

				scale.x = HMLfile["models"][modelID]["attributes"]["scale"]["x"];
				scale.y = HMLfile["models"][modelID]["attributes"]["scale"]["y"];
				scale.z = HMLfile["models"][modelID]["attributes"]["scale"]["z"];

				scene.models[i]->transformation.Translate(position);
				scene.models[i]->transformation.Scale(scale);

			}

			for (size_t i = 0; i < HMLfile["lightCount"]; i++)
			{
				std::string LightID("light" + std::to_string(i));

				glm::vec3 position(HMLfile["lights"][LightID]["attributes"]["position"]["x"],
					HMLfile["lights"][LightID]["attributes"]["position"]["y"],
					HMLfile["lights"][LightID]["attributes"]["position"]["z"]);

				glm::vec4 color(HMLfile["lights"][LightID]["attributes"]["color"]["r"],
					HMLfile["lights"][LightID]["attributes"]["color"]["g"],
					HMLfile["lights"][LightID]["attributes"]["color"]["b"],
					HMLfile["lights"][LightID]["attributes"]["color"]["a"]);

				float intensity = HMLfile["lights"][LightID]["attributes"]["intensity"];

				scene.Addlight(position, glm::vec3(1.0f), color, lightShader, CUBE_LIGHT, POINT_LIGHT);
				scene.LightIntensities[i] = intensity;

			}

			scene.handlelights(shader);

			data.EnableSSAO = HMLfile["GeneralAttributes"]["SSAO"]["enableSSAO"];
			data.SSAObias = HMLfile["GeneralAttributes"]["SSAO"]["SSAObias"];
			data.SSAOkernelSize = HMLfile["GeneralAttributes"]["SSAO"]["SSAOkernelSize"];
			data.SSAOradius = HMLfile["GeneralAttributes"]["SSAO"]["SSAOradius"];

			data.DOFenabled = HMLfile["GeneralAttributes"]["DOF"]["DOFenabled"];
			data.DOFfarDistance = HMLfile["GeneralAttributes"]["DOF"]["DOFfarDistance"];
			data.DOFintensity = HMLfile["GeneralAttributes"]["DOF"]["DOFintensity"];

			data.FogEnabled = HMLfile["GeneralAttributes"]["Fog"]["FogEnabled"];
			data.FogIntensity = HMLfile["GeneralAttributes"]["Fog"]["FogIntensity"];

			data.FogColor.x = HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["x"];
			data.FogColor.y = HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["y"];
			data.FogColor.z = HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["z"];

			data.RenderGrid = HMLfile["GeneralAttributes"]["renderGrid"];

			data.render_cube_map = HMLfile["GeneralAttributes"]["HDRI"]["enableSkybox"];
			data.HDRIpath = HMLfile["GeneralAttributes"]["HDRI"]["HDRIpath"];

			if (!data.HDRIpath.empty())
			{
				ImportCubeMap(cubemap, HDRItoCubeMapShader, ConvolutateCubeMapShader, PrefilterHDRIShader, data, logs);
			}

			data.renderlights = HMLfile["GeneralAttributes"]["showLightMeshes"];
			data.RenderShadows = HMLfile["GeneralAttributes"]["renderShadows"];

			ActiveCameraID = HMLfile["ViewportAttributes"]["ActiveCameraID"];

			LOG("SAVE FILE CAMERA COUNT: " << (int)HMLfile["ViewportAttributes"]["CameraCount"]);

			glm::vec3 TempCameraPos;
			glm::vec3 TempCameraOrien;

			for (size_t i = 0; i < (int)HMLfile["ViewportAttributes"]["CameraCount"]; i++)
			{
				std::string cameraID = "ID" + std::to_string(i);
				TempCameraPos.x = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["x"];
				TempCameraPos.y = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["y"];
				TempCameraPos.z = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["z"];

				TempCameraOrien.x = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["x"];
				TempCameraOrien.y = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["y"];
				TempCameraOrien.z = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["z"];

				AddCamera(TempCameraPos, TempCameraOrien);
			}

			data.CameraFOV = HMLfile["ViewportAttributes"]["cameraFOV"];
			renderPass = HMLfile["ViewportAttributes"]["renderPass"];

			LOG_INF("hml file read :: " << fileName);
			std::string logtemp = "hml file read :: " + std::string(fileName);
			logs.push_back(logtemp);
			glfwSetWindowTitle(window,("Hemlock Standalone Renderer - " + std::string(fileName)).c_str());
			File.close();
		}
		catch (const std::exception& e)
		{
			LOG_ERR("Exception while reading hml file :: " << e.what());
			std::string logtemp = "Exception while reading hml file :: " + std::string(e.what()) + " :: " + std::string(fileName);
			LOG_TO_FILE("Exception_log.txt", logtemp);
			logs.push_back(logtemp);
			File.close();

			for (size_t i = 0; i < scene.numberoflights; i++)
			{
				scene.DeleteLight(i, shader);
			}

		}
	}
	else
	{
		LOG_ERR("Unable to open the file :: " << fileName);
		LOG_TO_FILE("Exception_log.txt", "Unable to open the file :: " << fileName << " :: " << __TIME__ << " :: " << __DATE__);
		std::string logtemp = "Unable to open the file :: " + std::string(fileName);
		logs.push_back(logtemp);
	}

}

void SAVEFILE::ReadHMLfilePacked(const char* fileName, scene& scene, GLuint shader, GLuint lightShader, DATA::UIdataPack& data, Camera& camera, int& renderPass, std::vector<std::string>& logs, 
	GLFWwindow* window, CubeMap& cubemap, GLuint& HDRItoCubeMapShader, GLuint& ConvolutateCubeMapShader, GLuint& PrefilterHDRIShader)
{

	std::string HMLfileDirectory(fileName);
	int lastSlash = 0;

	for (size_t i = 0; i < HMLfileDirectory.size(); i++)
	{
		if (HMLfileDirectory[i] == '/')
		{
			lastSlash = i;
		}
	}

	HMLfileDirectory = HMLfileDirectory.substr(0, lastSlash);
	std::string TextureDirectory = "";
	std::string temp(HMLfileDirectory);

	for (size_t i = 0; i < temp.size(); i++)
	{
		if (temp.at(i) == '/')
		{
			TextureDirectory += '\\';
		}
		else
		{
			TextureDirectory += temp.at(i);
		}
	}

	LOG("TextureDirectory " << TextureDirectory);

	std::ifstream File(fileName);

	if (File.is_open())
	{
		try
		{
			ClearScene(scene, data, shader);
			json HMLfile;
			File >> HMLfile;

			for (size_t i = 1; i < HMLfile["modelCount"]; i++)
			{
				std::string modelID("model" + std::to_string(i));

				std::string ModelDirectory(HMLfileDirectory + "/" + std::string(HMLfile["models"][modelID]["directory"]));
				LOG("MODEL DIRECTORY " << ModelDirectory << " " << ModelDirectory.size());
				scene.ImportModel(ModelDirectory, shader);

				for (size_t meshid = 0; meshid < scene.models[i]->meshes.size(); meshid++)
				{
					std::string meshidname("mesh" + std::to_string(meshid));

					for (size_t textureid = 0; textureid < HMLfile["models"][modelID][meshidname]["textureCount"]; textureid++)
					{
						std::string texturePath(TextureDirectory + "\\" + std::string(HMLfile["models"][modelID][meshidname]["textures"][textureid]["directory"]));
						std::string textureType(HMLfile["models"][modelID][meshidname]["textures"][textureid]["type"]);

						scene.ImportTextureIntoModel(texturePath.c_str(), i, meshid, textureType.c_str());
					}
				}

				glm::vec3 position;
				glm::vec3 scale;

				position.x = HMLfile["models"][modelID]["attributes"]["position"]["x"];
				position.y = HMLfile["models"][modelID]["attributes"]["position"]["y"];
				position.z = HMLfile["models"][modelID]["attributes"]["position"]["z"];

				scale.x = HMLfile["models"][modelID]["attributes"]["scale"]["x"];
				scale.y = HMLfile["models"][modelID]["attributes"]["scale"]["y"];
				scale.z = HMLfile["models"][modelID]["attributes"]["scale"]["z"];

				scene.models[i]->transformation.Translate(position);
				scene.models[i]->transformation.Scale(scale);

			}

			for (size_t i = 0; i < HMLfile["lightCount"]; i++)
			{
				std::string LightID("light" + std::to_string(i));

				glm::vec3 position(HMLfile["lights"][LightID]["attributes"]["position"]["x"],
					HMLfile["lights"][LightID]["attributes"]["position"]["y"],
					HMLfile["lights"][LightID]["attributes"]["position"]["z"]);

				glm::vec4 color(HMLfile["lights"][LightID]["attributes"]["color"]["r"],
					HMLfile["lights"][LightID]["attributes"]["color"]["g"],
					HMLfile["lights"][LightID]["attributes"]["color"]["b"],
					HMLfile["lights"][LightID]["attributes"]["color"]["a"]);

				float intensity = HMLfile["lights"][LightID]["attributes"]["intensity"];

				scene.Addlight(position, glm::vec3(1.0f), color, lightShader, CUBE_LIGHT, POINT_LIGHT);
				scene.LightIntensities[i] = intensity;

			}

			scene.handlelights(shader);

			data.EnableSSAO = HMLfile["GeneralAttributes"]["SSAO"]["enableSSAO"];
			data.SSAObias = HMLfile["GeneralAttributes"]["SSAO"]["SSAObias"];
			data.SSAOkernelSize = HMLfile["GeneralAttributes"]["SSAO"]["SSAOkernelSize"];
			data.SSAOradius = HMLfile["GeneralAttributes"]["SSAO"]["SSAOradius"];

			data.DOFenabled = HMLfile["GeneralAttributes"]["DOF"]["DOFenabled"];
			data.DOFfarDistance = HMLfile["GeneralAttributes"]["DOF"]["DOFfarDistance"];
			data.DOFintensity = HMLfile["GeneralAttributes"]["DOF"]["DOFintensity"];

			data.FogEnabled = HMLfile["GeneralAttributes"]["Fog"]["FogEnabled"];
			data.FogIntensity = HMLfile["GeneralAttributes"]["Fog"]["FogIntensity"];

			data.FogColor.x = HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["x"];
			data.FogColor.y = HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["y"];
			data.FogColor.z = HMLfile["GeneralAttributes"]["Fog"]["FogColor"]["z"];

			data.RenderGrid = HMLfile["GeneralAttributes"]["renderGrid"];

			data.render_cube_map = HMLfile["GeneralAttributes"]["HDRI"]["enableSkybox"];
			data.HDRIpath = HMLfile["GeneralAttributes"]["HDRI"]["HDRIpath"];

			if (!data.HDRIpath.empty())
			{
				ImportCubeMap(cubemap, HDRItoCubeMapShader, ConvolutateCubeMapShader, PrefilterHDRIShader, data, logs);
			}

			data.renderlights = HMLfile["GeneralAttributes"]["showLightMeshes"];
			data.RenderShadows = HMLfile["GeneralAttributes"]["renderShadows"];

			ActiveCameraID = HMLfile["ViewportAttributes"]["ActiveCameraID"];

			glm::vec3 TempCameraPos;
			glm::vec3 TempCameraOrien;

			for (size_t i = 0; i < (int)HMLfile["ViewportAttributes"]["CameraCount"]; i++)
			{
				std::string cameraID = "ID" + std::to_string(i);
				TempCameraPos.x = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["x"];
				TempCameraPos.y = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["y"];
				TempCameraPos.z = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraPosition"]["z"];

				TempCameraOrien.x = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["x"];
				TempCameraOrien.y = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["y"];
				TempCameraOrien.z = HMLfile["ViewportAttributes"]["Cameras"][cameraID]["cameraOrientation"]["z"];

				AddCamera(TempCameraPos, TempCameraOrien);
			}

			data.CameraFOV = HMLfile["ViewportAttributes"]["cameraFOV"];
			renderPass = HMLfile["ViewportAttributes"]["renderPass"];

			LOG_INF("hml file read :: " << fileName);
			std::string logtemp = "hml file read :: " + std::string(fileName);
			logs.push_back(logtemp);
			glfwSetWindowTitle(window, ("Hemlock Standalone Renderer - " + std::string(fileName)).c_str());
			File.close();
		}
		catch (const std::exception& e)
		{
			LOG_ERR("Exception while reading packed hml file :: " << e.what());
			std::string logtemp = "Exception while reading packed hml file :: " + std::string(e.what()) + " :: " + std::string(fileName);
			logs.push_back(logtemp);
			File.close();
		}
	}
	else
	{
		LOG_ERR("Unable to open the file :: " << fileName);
		LOG_TO_FILE("Exception_log.txt", "Unable to open the file :: " << fileName << " :: " << __TIME__ << " :: " << __DATE__);
		std::string logtemp = "Unable to open the file :: " + std::string(fileName);
		logs.push_back(logtemp);
	}
}
