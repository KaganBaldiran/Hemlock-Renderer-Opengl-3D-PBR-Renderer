#pragma once
#include <glew.h>
#include <glfw3.h>
#include "VectorMath.h"
#include "Log.h"
#include "Model.h"
#include <functional>
#include "Camera.h"

namespace BAKER
{
	std::pair<GLuint, int> BakeNormal(GLuint NormalCubeMapSize, Model* highPolyModel, Model* lowPolyModel, GLuint HighPolyShader, GLuint LowPolyShader , Camera& camera);





}