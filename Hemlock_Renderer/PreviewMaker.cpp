#include "PreviewMaker.hpp"
#include "post_process.h"

void PREVIEW::CreatePreviewForModel(GLuint& PreviewTexture, const char* FilePath , Shader& PreviewShader , Model** PreviewModelPointer)
{
	FBO fbo;
	const int WindowWidth = 1024;
	const int WindowHeight = 1024;

	std::string Path(FilePath);

	for (size_t i = 0; i < Path.size(); i++)
	{
		if (Path[i] == '\\') {
			Path[i] = '/';
		}
	}

	Model *PreviewModel = new Model(Path, false);
	LOG("NEW MODEL NAME: " << PreviewModel->ModelName);
	PreviewModel->FindGlobalMeshScales();

	CreateCustomFrameBuffer(fbo, WindowWidth, WindowHeight);
	Camera PreviewCamera(WindowWidth, WindowHeight, glm::vec3(glm::min(PreviewModel->transformation.ObjectScales.x, PreviewModel->transformation.ObjectScales.z),
		                                            PreviewModel->transformation.ObjectScales.y * 1.3f,
		                                            glm::max( PreviewModel->transformation.ObjectScales.x , PreviewModel->transformation.ObjectScales.z)));

	PreviewCamera.Orientation = glm::vec3(PreviewModel->originpoint.x, PreviewModel->originpoint.y, PreviewModel->originpoint.z) - PreviewCamera.Position;
	PreviewCamera.updateMatrix(45.0f, 0.1f, 1000.0f, { WindowWidth,WindowHeight });

	fbo.Bind(GL_FRAMEBUFFER);
	PreviewShader.use();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WindowWidth, WindowHeight);
	auto ShaderPrep = [&]() {
		PreviewModel->transformation.SetModelMatrixUniformLocation(PreviewShader.GetID(), "model");
		PreviewShader.setVec3("CamPos", PreviewCamera.Get_Position());
		PreviewShader.setVec3("ObjectScales", PreviewModel->transformation.ObjectScales);
		PreviewShader.setFloat("FarNearPlaneDifference", PreviewCamera.farPlane - PreviewCamera.farPlane);
	};
	PreviewModel->DrawMultipleShadowMaps(PreviewShader.GetID(),PreviewCamera, ShaderPrep , NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	PreviewTexture = fbo.GetScreenImage();
	UseShaderProgram(0);
	fbo.DeleteFBO();
	fbo.DeleteRBO();
	*PreviewModelPointer = PreviewModel;
}
