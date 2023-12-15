#pragma once
#include <glew.h>
#include <glfw3.h>
#include "Model.h"
#include "Thread.h"

namespace PREVIEW
{
	void CreatePreviewForModel(GLuint& PreviewTexture, const char* FilePath, Shader& PreviewShader, Model** PreviewModelPointer);
}

