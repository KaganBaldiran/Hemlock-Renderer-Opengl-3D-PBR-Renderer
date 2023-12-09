#include "Transformation.hpp"


void WorldTransform::SetModelMatrixUniformLocation(GLuint shader, const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader, uniform), 1, GL_FALSE, glm::value_ptr(GetModelMat4()));
}

void WorldTransform::Translate(glm::vec3 v)
{
	TranslationMatrix = glm::translate(TranslationMatrix, v * ScaleFactor);

	Position.x = TranslationMatrix[3][0];
	Position.y = TranslationMatrix[3][1];
	Position.z = TranslationMatrix[3][2];
}

void WorldTransform::Scale(glm::vec3 v)
{
	ScalingMatrix = glm::scale(ScalingMatrix, v);
	ObjectScales *= v;
	ScaleFactor *= v;
}

void WorldTransform::Rotate(float angle, glm::vec3 v)
{
	RotationMatrix = glm::rotate(RotationMatrix, glm::radians(angle), v);
}

void WorldTransform::TranslateNoTraceBack(glm::vec3 v)
{
	TranslationMatrix = glm::translate(TranslationMatrix, v);

	Position.x = TranslationMatrix[3][0];
	Position.y = TranslationMatrix[3][1];
	Position.z = TranslationMatrix[3][2];
}

void WorldTransform::ScaleNoTraceBack(glm::vec3 v)
{
	ScalingMatrix = glm::scale(ScalingMatrix, v);
	ObjectScales *= v;
	ScaleFactor *= v;
}

void WorldTransform::RotateNoTraceBack(glm::vec3 v, float angle)
{
	RotationMatrix = glm::rotate(RotationMatrix, glm::radians(angle), v);
}