#pragma once
#include <glew.h>
#include <glfw3.h>
#include "Model.h"

namespace PREVIEW
{
	void CreatePreviewForModel(GLuint& PreviewTexture, const char* FilePath, Shader& PreviewShader);
}

