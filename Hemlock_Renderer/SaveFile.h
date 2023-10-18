#pragma once
#include <glew.h>
#include <glfw3.h>
#include "../dependencies/nlohmann/json.hpp"
#include "VectorMath.h"
#include "Log.h"
#include <algorithm>
using json = nlohmann::json;


namespace SAVEFILE
{
	struct SaveFileData
	{
		int ViewportTheme = -1;
		int HDRIsize = -1;
		int CameraLayout = -1;

		bool empty()
		{
			return ViewportTheme == -1 && HDRIsize == -1 && CameraLayout == -1;
		}

	};

	bool ReadSaveFile(const char* fileName, SaveFileData& data);
	void WriteSaveFile(const char* fileName, SaveFileData& data);

}