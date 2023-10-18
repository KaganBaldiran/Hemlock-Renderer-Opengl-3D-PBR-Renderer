#include "SaveFile.h"
#include <fstream>

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
