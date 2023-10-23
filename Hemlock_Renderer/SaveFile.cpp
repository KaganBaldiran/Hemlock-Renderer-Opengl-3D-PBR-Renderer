#include "SaveFile.h"
#include <fstream>
#include "Texture.h"

bool IsFileEmpty(std::ifstream& file) {
	return file.peek() == std::ifstream::traits_type::eof();
}

bool SAVEFILE::ReadSaveFile(const char* fileName, SaveFileData& data)
{
	std::ifstream File(fileName);

	if (File.is_open())
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

				LOG_INF("Read the preferences!");
			}
			
		}
		else
		{
			return false;
			LOG_INF("Preferences file is empty!");
		}
	}
	else
	{
		return false;
		LOG_ERR("Unable to open the preferences file to read!");
	}

	return true;
}

void SAVEFILE::WriteSaveFile(const char* fileName, SaveFileData& data)
{
	if (!data.empty())
	{
		std::ofstream File(fileName);

		if (File.is_open())
		{
			json SaveFile;

			SaveFile["HDRIsize"] = data.HDRIsize;
			SaveFile["ViewportTheme"] = data.ViewportTheme;
			SaveFile["CameraLayout"] = data.CameraLayout;

			File << SaveFile;

			File.close();
			LOG_INF("Prefences saved!");
		}
		else
		{
			LOG_ERR("Unable to open the preferences file!");
		}
	}

}


void SAVEFILE::WriteHMLfile(const char* fileName, scene& scene ,UIdataPack& data,Camera& camera , int renderPass, std::vector<std::string>& logs)
{
	std::ofstream File(fileName);

	if (File.is_open())
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


		HMLfile["GeneralAttributes"]["enableSSAO"] = data.EnableSSAO;
		HMLfile["GeneralAttributes"]["renderGrid"] = data.RenderGrid;
		HMLfile["GeneralAttributes"]["enableSkybox"] = data.render_cube_map;
		HMLfile["GeneralAttributes"]["showLightMeshes"] = data.renderlights;

		HMLfile["ViewportAttributes"]["cameraPosition"]["x"] = camera.Position.x;
		HMLfile["ViewportAttributes"]["cameraPosition"]["y"] = camera.Position.y;
		HMLfile["ViewportAttributes"]["cameraPosition"]["z"] = camera.Position.z;

		HMLfile["ViewportAttributes"]["cameraOrientation"]["x"] = camera.Orientation.x;
		HMLfile["ViewportAttributes"]["cameraOrientation"]["y"] = camera.Orientation.y;
		HMLfile["ViewportAttributes"]["cameraOrientation"]["z"] = camera.Orientation.z;

		HMLfile["ViewportAttributes"]["renderPass"] = renderPass;

		File << HMLfile;
		LOG_INF("hml file saved :: " << fileName);
		std::string logtemp = "hml file saved :: " + std::string(fileName);
		logs.push_back(logtemp);
		File.close();
	}
	else
	{
		LOG_ERR("Unable to open the file :: " << fileName);
		std::string logtemp = "Unable to open the file :: " + std::string(fileName);
		logs.push_back(logtemp);
	}

}

void SAVEFILE::ReadHMLfile(const char* fileName, scene& scene , GLuint shader ,GLuint lightShader, UIdataPack& data, Camera& camera, int& renderPass, std::vector<std::string> &logs)
{
	std::ifstream File(fileName);

	if (File.is_open())
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
					Texture newTexturePush;
					newTexturePush.id = *newTexture.GetTexture();
					newTexturePush.path = newTexture.GetPathData();
					newTexturePush.type = textureType;

					scene.models[i]->meshes[meshid].textures.push_back(newTexturePush);
					scene.models[i]->textures_loaded.push_back(newTexturePush);
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

		data.EnableSSAO = HMLfile["GeneralAttributes"]["enableSSAO"];
		data.RenderGrid = HMLfile["GeneralAttributes"]["renderGrid"];
		data.render_cube_map = HMLfile["GeneralAttributes"]["enableSkybox"];
		data.renderlights = HMLfile["GeneralAttributes"]["showLightMeshes"];

		camera.Position.x = HMLfile["ViewportAttributes"]["cameraPosition"]["x"];
		camera.Position.y = HMLfile["ViewportAttributes"]["cameraPosition"]["y"];
		camera.Position.z = HMLfile["ViewportAttributes"]["cameraPosition"]["z"];

		camera.Orientation.x = HMLfile["ViewportAttributes"]["cameraOrientation"]["x"];
		camera.Orientation.y = HMLfile["ViewportAttributes"]["cameraOrientation"]["y"];
		camera.Orientation.z = HMLfile["ViewportAttributes"]["cameraOrientation"]["z"];

		renderPass = HMLfile["ViewportAttributes"]["renderPass"];

		LOG_INF("hml file read :: " << fileName);
		std::string logtemp = "hml file read :: " + std::string(fileName);
		logs.push_back(logtemp);
		File.close();
	}
	else
	{
		LOG_ERR("Unable to open the file :: " << fileName);
		std::string logtemp = "Unable to open the file :: " + std::string(fileName);
		logs.push_back(logtemp);
	}

}
