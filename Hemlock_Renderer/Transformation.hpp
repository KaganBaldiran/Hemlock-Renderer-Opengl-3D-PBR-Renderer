#pragma once
#include <glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "VectorMath.h"

class WorldTransform
{
public:

    WorldTransform()
    {
        Position.x = TranslationMatrix[3][0];
        Position.y = TranslationMatrix[3][1];
        Position.z = TranslationMatrix[3][2];
        ScaleFactor = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    void operator()(const WorldTransform& other)
    {
        this->Position = other.Position;
        TranslationMatrix = other.TranslationMatrix;
        RotationMatrix = other.RotationMatrix;
        ScalingMatrix = other.ScalingMatrix;
        
        ObjectScales = other.ObjectScales;
        InitialObjectScales = other.InitialObjectScales;
        scale_avg = other.scale_avg;
        ScaleFactor = other.ScaleFactor;
    }

    glm::mat4 TranslationMatrix = glm::mat4(1.0f);
    glm::mat4 RotationMatrix = glm::mat4(1.0f);
    glm::mat4 ScalingMatrix = glm::mat4(1.0f);

    glm::vec3 ObjectScales;
    glm::vec3 InitialObjectScales;

    glm::vec3 Position;
    float scale_avg;
    float dynamic_scale_avg;

    glm::vec3 ScaleFactor;

    void SetModelMatrixUniformLocation(GLuint shader, const char* uniform);
    void Translate(glm::vec3 v);
    void Scale(glm::vec3 v);
    void Rotate(float angle, glm::vec3 v);

    //No transform history for children
    void TranslateNoTraceBack(glm::vec3 v);
    void ScaleNoTraceBack(glm::vec3 v);
    void RotateNoTraceBack(glm::vec3 v, float angle);

    glm::mat4 GetModelMat4()
    {
        return TranslationMatrix * RotationMatrix * ScalingMatrix;
    };
};