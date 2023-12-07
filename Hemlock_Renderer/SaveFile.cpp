#include "SaveFile.h"
#include <filesystem>
#include <fstream>
#include "Texture.h"
#include <iostream>

bool IsFileEmpty(std::ifstream& file) {
	return file.peek() == std::ifstream::traits_type::eof();
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

			for (size_t i = 1; i < scene.GetModelCount(); i++)
			{
				HMLfile["models"][i]["directory"] = "models" + std::string(strrchr(scene.models[i]->ModelImportPath.c_str(), '/'));
				LOG("MESH NAME: " << "models" + std::string(strrchr(scene.models[i]->ModelImportPath.c_str(), '/')));

				std::filesystem::copy(scene.models[i]->modelpath, ModelsDirectory, std::filesystem::copy_options::recursive);

				for (size_t meshid = 0; meshid < scene.models[i]->meshes.size(); meshid++)
				{
					std::string meshidname("mesh" + std::to_string(meshid));

					HMLfile["models"][i][meshidname]["textureCount"] = scene.models[i]->meshes[meshid].textures.size();

					for (size_t textureid = 0; textureid < scene.models[i]->meshes[meshid].textures.size(); textureid++)
					{
						HMLfile["models"][i][meshidname]["textures"][textureid]["directory"] = "textures" + std::string(strrchr(scene.models[i]->meshes[meshid].textures[textureid].path.c_str(), '\\'));
						std::filesystem::copy(scene.models[i]->meshes[meshid].textures[textureid].path, TexturesDirectory, std::filesystem::copy_options::recursive);
						LOG("TEXTURE DIRECTORY: " << "textures\\" + std::string(strrchr(scene.models[i]->meshes[meshid].textures[textureid].path.c_str(), '\\')));
						HMLfile["models"][i][meshidname]["textures"][textureid]["type"] = scene.models[i]->meshes[meshid].textures[textureid].type;
					}
				}

				HMLfile["models"][i]["attributes"]["position"]["x"] = scene.models[i]->transformation.transformmatrix[3][0];
				HMLfile["models"][i]["attributes"]["position"]["y"] = scene.models[i]->transformation.transformmatrix[3][1];
				HMLfile["models"][i]["attributes"]["position"]["z"] = scene.models[i]->transformation.transformmatrix[3][2];

				HMLfile["models"][i]["attributes"]["scale"]["x"] = scene.models[i]->transformation.transformmatrix[0][0];
				HMLfile["models"][i]["attributes"]["scale"]["y"] = scene.models[i]->transformation.transformmatrix[1][1];
				HMLfile["models"][i]["attributes"]["scale"]["z"] = scene.models[i]->transformation.transformmatrix[2][2];
			}


			HMLfile["lightCount"] = scene.numberoflights;

			for (size_t i = 0; i < scene.numberoflights; i++)
			{
				HMLfile["lights"][i]["attributes"]["position"]["x"] = scene.LightPositions[i].x;
				HMLfile["lights"][i]["attributes"]["position"]["y"] = scene.LightPositions[i].y;
				HMLfile["lights"][i]["attributes"]["position"]["z"] = scene.LightPositions[i].z;

				HMLfile["lights"][i]["attributes"]["intensity"] = scene.LightIntensities[i];

				HMLfile["lights"][i]["attributes"]["color"]["r"] = scene.LightColors[i].x;
				HMLfile["lights"][i]["attributes"]["color"]["g"] = scene.LightColors[i].y;
				HMLfile["lights"][i]["attributes"]["color"]["b"] = scene.LightColors[i].z;
				HMLfile["lights"][i]["attributes"]["color"]["a"] = scene.LightColors[i].w;
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

			HMLfile["ViewportAttributes"]["cameraPosition"]["x"] = camera.Position.x;
			HMLfile["ViewportAttributes"]["cameraPosition"]["y"] = camera.Position.y;
			HMLfile["ViewportAttributes"]["cameraPosition"]["z"] = camera.Position.z;

			HMLfile["ViewportAttributes"]["cameraOrientation"]["x"] = camera.Orientation.x;
			HMLfile["ViewportAttributes"]["cameraOrientation"]["y"] = camera.Orientation.y;
			HMLfile["ViewportAttributes"]["cameraOrientation"]["z"] = camera.Orientation.z;

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

			for (size_t i = 1; i < scene.GetModelCount(); i++)
			{
				HMLfile["models"][i]["directory"] = scene.models[i]->ModelImportPath;


				for (size_t meshid = 0; meshid < scene.models[i]->meshes.size(); meshid++)
				{
					std::string meshidname("mesh" + std::to_string(meshid));

					HMLfile["models"][i][meshidname]["textureCount"] = scene.models[i]->meshes[meshid].textures.size();

					for (size_t textureid = 0; textureid < scene.models[i]->meshes[meshid].textures.size(); textureid++)
					{
						HMLfile["models"][i][meshidname]["textures"][textureid]["directory"] = scene.models[i]->meshes[meshid].textures[textureid].path;
						HMLfile["models"][i][meshidname]["textures"][textureid]["type"] = scene.models[i]->meshes[meshid].textures[textureid].type;
					}
				}

				for (int row = 0; row < 4; row++) {
					for (int col = 0; col < 4; col++) {
						cout << "Element [" << row << "][" << col << "]: " << scene.models[i]->transformation.transformmatrix[row][col] << endl;
					}
				}

				HMLfile["models"][i]["attributes"]["position"]["x"] = scene.models[i]->transformation.transformmatrix[3][0];
				HMLfile["models"][i]["attributes"]["position"]["y"] = scene.models[i]->transformation.transformmatrix[3][1];
				HMLfile["models"][i]["attributes"]["position"]["z"] = scene.models[i]->transformation.transformmatrix[3][2];

				HMLfile["models"][i]["attributes"]["scale"]["x"] = scene.models[i]->transformation.transformmatrix[0][0];
				HMLfile["models"][i]["attributes"]["scale"]["y"] = scene.models[i]->transformation.transformmatrix[1][1];
				HMLfile["models"][i]["attributes"]["scale"]["z"] = scene.models[i]->transformation.transformmatrix[2][2];

			}

			HMLfile["lightCount"] = scene.numberoflights;

			for (size_t i = 0; i < scene.numberoflights; i++)
			{
				HMLfile["lights"][i]["attributes"]["position"]["x"] = scene.LightPositions[i].x;
				HMLfile["lights"][i]["attributes"]["position"]["y"] = scene.LightPositions[i].y;
				HMLfile["lights"][i]["attributes"]["position"]["z"] = scene.LightPositions[i].z;

				HMLfile["lights"][i]["attributes"]["intensity"] = scene.LightIntensities[i];

				HMLfile["lights"][i]["attributes"]["color"]["r"] = scene.LightColors[i].x;
				HMLfile["lights"][i]["attributes"]["color"]["g"] = scene.LightColors[i].y;
				HMLfile["lights"][i]["attributes"]["color"]["b"] = scene.LightColors[i].z;
				HMLfile["lights"][i]["attributes"]["color"]["a"] = scene.LightColors[i].w;
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

			HMLfile["ViewportAttributes"]["cameraPosition"]["x"] = camera.Position.x;
			HMLfile["ViewportAttributes"]["cameraPosition"]["y"] = camera.Position.y;
			HMLfile["ViewportAttributes"]["cameraPosition"]["z"] = camera.Position.z;

			HMLfile["ViewportAttributes"]["cameraOrientation"]["x"] = camera.Orientation.x;
			HMLfile["ViewportAttributes"]["cameraOrientation"]["y"] = camera.Orientation.y;
			HMLfile["ViewportAttributes"]["cameraOrientation"]["z"] = camera.Orientation.z;

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
			json HMLfile;
			File >> HMLfile;

			for (size_t i = 1; i < HMLfile["modelCount"]; i++)
			{
				scene.ImportModel(HMLfile["models"][i]["directory"], shader);

				for (size_t meshid = 0; meshid < scene.models[i]->meshes.size(); meshid++)
				{
					std::string meshidname("mesh" + std::to_string(meshid));

					for (size_t textureid = 0; textureid < HMLfile["models"][i][meshidname]["textureCount"]; textureid++)
					{
						std::string texturePath(HMLfile["models"][i][meshidname]["textures"][textureid]["directory"]);
						std::string textureType(HMLfile["models"][i][meshidname]["textures"][textureid]["type"]);

						Textures newTexture(texturePath.c_str(), textureid, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, NULL, textureType);
						if (newTexture.GetTextureState() == TEXTURE_SUCCESS)
						{
							Texture newTexturePush;
							newTexturePush.id = *newTexture.GetTexture();
							newTexturePush.path = newTexture.GetPathData();
							newTexturePush.type = textureType;

							scene.models[i]->meshes[meshid].textures.push_back(newTexturePush);
							scene.models[i]->textures_loaded.push_back(newTexturePush);
						}
					}
				}

				glm::vec3 position;
				glm::vec3 scale;

				position.x = HMLfile["models"][i]["attributes"]["position"]["x"];
				position.y = HMLfile["models"][i]["attributes"]["position"]["y"];
				position.z = HMLfile["models"][i]["attributes"]["position"]["z"];

				scale.x = HMLfile["models"][i]["attributes"]["scale"]["x"];
				scale.y = HMLfile["models"][i]["attributes"]["scale"]["y"];
				scale.z = HMLfile["models"][i]["attributes"]["scale"]["z"];

				scene.models[i]->transformation.translate(position);
				scene.models[i]->transformation.scale(scale);

			}

			for (size_t i = 0; i < scene.numberoflights; i++)
			{
				scene.DeleteLight(i, shader);
			}

			for (size_t i = 0; i < HMLfile["lightCount"]; i++)
			{

				glm::vec3 position(HMLfile["lights"][i]["attributes"]["position"]["x"],
					HMLfile["lights"][i]["attributes"]["position"]["y"],
					HMLfile["lights"][i]["attributes"]["position"]["z"]);

				glm::vec4 color(HMLfile["lights"][i]["attributes"]["color"]["r"],
					HMLfile["lights"][i]["attributes"]["color"]["g"],
					HMLfile["lights"][i]["attributes"]["color"]["b"],
					HMLfile["lights"][i]["attributes"]["color"]["a"]);

				float intensity = HMLfile["lights"][i]["attributes"]["intensity"];

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

			camera.Position.x = HMLfile["ViewportAttributes"]["cameraPosition"]["x"];
			camera.Position.y = HMLfile["ViewportAttributes"]["cameraPosition"]["y"];
			camera.Position.z = HMLfile["ViewportAttributes"]["cameraPosition"]["z"];

			camera.Orientation.x = HMLfile["ViewportAttributes"]["cameraOrientation"]["x"];
			camera.Orientation.y = HMLfile["ViewportAttributes"]["cameraOrientation"]["y"];
			camera.Orientation.z = HMLfile["ViewportAttributes"]["cameraOrientation"]["z"];

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

			json HMLfile;
			File >> HMLfile;

			for (size_t i = 1; i < HMLfile["modelCount"]; i++)
			{
				std::string ModelDirectory(HMLfileDirectory + "/" + std::string(HMLfile["models"][i]["directory"]));
				LOG("MODEL DIRECTORY " << ModelDirectory << " " << ModelDirectory.size());
				scene.ImportModel(ModelDirectory, shader);

				for (size_t meshid = 0; meshid < scene.models[i]->meshes.size(); meshid++)
				{
					std::string meshidname("mesh" + std::to_string(meshid));

					for (size_t textureid = 0; textureid < HMLfile["models"][i][meshidname]["textureCount"]; textureid++)
					{
						std::string texturePath(TextureDirectory + "\\" + std::string(HMLfile["models"][i][meshidname]["textures"][textureid]["directory"]));
						std::string textureType(HMLfile["models"][i][meshidname]["textures"][textureid]["type"]);

						Textures newTexture(texturePath.c_str(), textureid, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, NULL, textureType);
						if (newTexture.GetTextureState() == TEXTURE_SUCCESS)
						{
							Texture newTexturePush;
							newTexturePush.id = *newTexture.GetTexture();
							newTexturePush.path = newTexture.GetPathData();
							newTexturePush.type = textureType;

							scene.models[i]->meshes[meshid].textures.push_back(newTexturePush);
							scene.models[i]->textures_loaded.push_back(newTexturePush);
						}
					}
				}

				glm::vec3 position;
				glm::vec3 scale;

				position.x = HMLfile["models"][i]["attributes"]["position"]["x"];
				position.y = HMLfile["models"][i]["attributes"]["position"]["y"];
				position.z = HMLfile["models"][i]["attributes"]["position"]["z"];

				scale.x = HMLfile["models"][i]["attributes"]["scale"]["x"];
				scale.y = HMLfile["models"][i]["attributes"]["scale"]["y"];
				scale.z = HMLfile["models"][i]["attributes"]["scale"]["z"];

				scene.models[i]->transformation.translate(position);
				scene.models[i]->transformation.scale(scale);

			}

			for (size_t i = 0; i < scene.numberoflights; i++)
			{
				scene.DeleteLight(i, shader);
			}

			for (size_t i = 0; i < HMLfile["lightCount"]; i++)
			{

				glm::vec3 position(HMLfile["lights"][i]["attributes"]["position"]["x"],
					HMLfile["lights"][i]["attributes"]["position"]["y"],
					HMLfile["lights"][i]["attributes"]["position"]["z"]);

				glm::vec4 color(HMLfile["lights"][i]["attributes"]["color"]["r"],
					HMLfile["lights"][i]["attributes"]["color"]["g"],
					HMLfile["lights"][i]["attributes"]["color"]["b"],
					HMLfile["lights"][i]["attributes"]["color"]["a"]);

				float intensity = HMLfile["lights"][i]["attributes"]["intensity"];

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

			camera.Position.x = HMLfile["ViewportAttributes"]["cameraPosition"]["x"];
			camera.Position.y = HMLfile["ViewportAttributes"]["cameraPosition"]["y"];
			camera.Position.z = HMLfile["ViewportAttributes"]["cameraPosition"]["z"];

			camera.Orientation.x = HMLfile["ViewportAttributes"]["cameraOrientation"]["x"];
			camera.Orientation.y = HMLfile["ViewportAttributes"]["cameraOrientation"]["y"];
			camera.Orientation.z = HMLfile["ViewportAttributes"]["cameraOrientation"]["z"];

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
