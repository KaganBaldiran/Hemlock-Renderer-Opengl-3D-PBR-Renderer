#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Meshhhh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "VectorMath.h"
#include <functional>
#include "Material.hpp"
#include "Transformation.hpp"

using namespace std;
using namespace MeshUtil;

extern uint ModelIDiterator;

inline unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class UIproperties
{
public:

    bool autorotate = false;
    float rotationamount = NULL;
    float keepoldrotation = NULL;
    Vec3<float> moveamount = { NULL,NULL,NULL };
    Vec3<float> maxmove = { 20.0f,20.0f,20.0f };
    Vec3<float> newtreshold = { 20.0f,20.0f,20.0f };
    float degree;
    float scaleamount = 1.0f;
    float maxscale = 2.0f;

    UIproperties& operator=(const UIproperties& other)
    {
        autorotate = other.autorotate;
        rotationamount = other.rotationamount;
        keepoldrotation = other.keepoldrotation;
        moveamount = other.moveamount;
        maxmove = other.maxmove;
        newtreshold = other.newtreshold;
        degree = other.degree;
        scaleamount = other.scaleamount;
        maxscale = other.maxscale;
        return *this;
    }

};


class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	
    vector<Mesh>  meshes;
    string directory;
    bool gammaCorrection;
    string modelpath;
    WorldTransform transformation;
    UIproperties UIprop;
    bool imported = false;
    uint* modelcounterptr;
    Vec3<double> originpoint;
    bool Selected = false;
    std::string ModelName;
    int SameModelInstances;
    std::string ModelImportPath;

    glm::vec3 dynamic_origin;

    bool gizmo_first_time = true;
    Material ObjectMaterial;
    
    Model(string const& path, bool gamma = false) : gammaCorrection(gamma) , SameModelInstances(0)
    {
        const char* modelName = strrchr(path.c_str(), '/');
        if (modelName != nullptr) {
            modelName++; 
            this->ModelName = modelName;
        }
        else {
            this->ModelName = path;
        }
        
        for (size_t i = 0; i < ModelName.size(); i++)
        {
            if (ModelName.at(i) == '.')
            {
                ModelName = ModelName.substr(0, i);
                break;
            }
        }

        LOG("ModelIDiterator: " << ModelIDiterator);

        modelcounterptr = &ModelIDiterator;
        modelid = ModelIDiterator;
        modelpath = path;
        loadModel(path);
        ModelIDiterator++;
        
    }

    Model(bool gamma = false) : gammaCorrection(gamma)
    {
        modelcounterptr = &ModelIDiterator;
        modelid = ModelIDiterator;
        ModelIDiterator++;
    }

    ~Model()
    {
        for (size_t i = 0; i < meshes.size(); i++)
        {
            meshes[i].CleanBuffer();
        }
        static long int model_count = NULL;
        LOG_INF("Model " <<model_count<< " buffers are cleaned :: "<< modelpath);
        model_count++;
    }

    void Draw(GLuint shader , Camera& camera, GLuint shadowMap , GLuint cube_map_texture)
    {
        UseShaderProgram(shader);
        glUniform1f(glGetUniformLocation(shader, "modelID"), this->GetModelID());

        glUniform1f(glGetUniformLocation(shader, "farPlane"), 25.0f);

        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader , camera,shadowMap,cube_map_texture);
    }

    void DrawMultipleShadowMaps(GLuint shader, Camera& camera, std::function<void()> ShaderPreperation, GLuint cube_map_texture)
    {
        UseShaderProgram(shader);

        ShaderPreperation();

        glUniform1f(glGetUniformLocation(shader, "modelID"), this->GetModelID());

        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader, camera, NULL, cube_map_texture);
    }

    void Draw(GLuint shader, Camera& camera, GLuint shadowMap, GLuint cube_map_texture , GLuint SSAOmap)
    {
        UseShaderProgram(shader);
        glUniform1f(glGetUniformLocation(shader, "modelID"), this->GetModelID());

        //glActiveTexture(GL_TEXTURE10);
        //glBindTexture(GL_TEXTURE_2D, cube_map_texture);
        //glUniform1i(glGetUniformLocation(shader, "SSAO"), 10);

        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader, camera, shadowMap, cube_map_texture);
    }


    void Draw(GLuint shader, std::function<void()> ShaderPreperation)
    {
        UseShaderProgram(shader);
        glUniform1f(glGetUniformLocation(shader, "modelID"), this->GetModelID());

        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader, ShaderPreperation);
    }

    void FindGlobalMeshScales()
    {

        float maxX = -std::numeric_limits<float>::infinity();
        float maxY = -std::numeric_limits<float>::infinity();
        float maxZ = -std::numeric_limits<float>::infinity();
        float minX = std::numeric_limits<float>::infinity();
        float minY = std::numeric_limits<float>::infinity();
        float minZ = std::numeric_limits<float>::infinity();

        for (size_t j = 0; j < meshes.size(); j++)
        {
            auto& VertexArray = meshes[j].vertices;
            Vertex origin = VertexArray[0];

            for (unsigned int k = 0; k < VertexArray.size(); k++) {

                Vertex vertex;
                vertex.Position.x = VertexArray[k].Position.x - origin.Position.x;
                vertex.Position.y = VertexArray[k].Position.y - origin.Position.y;
                vertex.Position.z = VertexArray[k].Position.z - origin.Position.z;

                maxX = std::max(maxX, vertex.Position.x);
                maxY = std::max(maxY, vertex.Position.y);
                maxZ = std::max(maxZ, vertex.Position.z);
                minX = std::min(minX, vertex.Position.x);
                minY = std::min(minY, vertex.Position.y);
                minZ = std::min(minZ, vertex.Position.z);
            }
        }


        float meshWidth = maxX - minX;
        float meshHeight = maxY - minY;
        float meshDepth = maxZ - minZ;

        transformation.ObjectScales.x = meshWidth;
        transformation.ObjectScales.y = meshHeight;
        transformation.ObjectScales.z = meshDepth;

        maxX = -std::numeric_limits<float>::infinity();
        maxY = -std::numeric_limits<float>::infinity();
        maxZ = -std::numeric_limits<float>::infinity();
        minX = std::numeric_limits<float>::infinity();
        minY = std::numeric_limits<float>::infinity();
        minZ = std::numeric_limits<float>::infinity();

        transformation.scale_avg = (transformation.ObjectScales.x + transformation.ObjectScales.y + transformation.ObjectScales.z) / 3;
        transformation.dynamic_scale_avg = transformation.scale_avg;

        transformation.ObjectScales.x = transformation.ObjectScales.x;
        transformation.ObjectScales.y = transformation.ObjectScales.y;
        transformation.ObjectScales.z = transformation.ObjectScales.z;

        transformation.InitialObjectScales = transformation.ObjectScales;

        std::cout << "Model width: " << transformation.ObjectScales.x << " Model height: " << transformation.ObjectScales.y << " Model Depth: " << transformation.ObjectScales.z << "\n";
        std::cout << "Scale avg: " << transformation.scale_avg << "\n";
    }


    int GetModelID() { return modelid; };

    int *GetModelIDptr() { return &modelid; };

    uint *GetModelIDcounterptr() { return modelcounterptr; };

private:

    int modelid;

    
    void loadModel(string const& path)
    {
        ModelImportPath = path;
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            LOG_ERR("ERROR::ASSIMP:: " << importer.GetErrorString());
            return;
        }


        std::vector<glm::vec3> originPoints;
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];
            

            float minX = mesh->mVertices[0].x, minY = mesh->mVertices[0].y, minZ = mesh->mVertices[0].z;
            float maxX = mesh->mVertices[0].x, maxY = mesh->mVertices[0].y, maxZ = mesh->mVertices[0].z;
            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                if (mesh->mVertices[j].x < minX) minX = mesh->mVertices[j].x;
                if (mesh->mVertices[j].y < minY) minY = mesh->mVertices[j].y;
                if (mesh->mVertices[j].z < minZ) minZ = mesh->mVertices[j].z;
                if (mesh->mVertices[j].x > maxX) maxX = mesh->mVertices[j].x;
                if (mesh->mVertices[j].y > maxY) maxY = mesh->mVertices[j].y;
                if (mesh->mVertices[j].z > maxZ) maxZ = mesh->mVertices[j].z;
            }
            float centerX = (minX + maxX) / 2.0f;
            float centerY = (minY + maxY) / 2.0f;
            float centerZ = (minZ + maxZ) / 2.0f;

            originPoints.push_back(glm::vec3(centerX, centerY, centerZ));
        }

        float overallCenterX = 0.0f, overallCenterY = 0.0f, overallCenterZ = 0.0f;
        for (unsigned int i = 0; i < originPoints.size(); i++) {
            overallCenterX += originPoints[i].x;
            overallCenterY += originPoints[i].y;
            overallCenterZ += originPoints[i].z;
        }
        overallCenterX /= originPoints.size();
        overallCenterY /= originPoints.size();
        overallCenterZ /= originPoints.size();

        originpoint = { overallCenterX,overallCenterY,overallCenterZ };

        dynamic_origin = glm::vec3(overallCenterX, overallCenterY, overallCenterZ);

        LOG_INF("Overall origin point: (" << overallCenterX << ", " << overallCenterY << ", " << overallCenterZ << ")");
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    
    void processNode(aiNode* node, const aiScene* scene)
    {
        
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; 
            
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
           
            if (mesh->mTextureCoords[0]) 
            {
                glm::vec2 vec;
                
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;

                
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
      
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
     

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        Mesh newmesh(vertices, indices, textures);

        newmesh.meshname = mesh->mName.C_Str();

        newmesh.diffuse_textures.assign(diffuseMaps.begin(), diffuseMaps.end());
        newmesh.specular_textures.assign(specularMaps.begin(), specularMaps.end());
        newmesh.normal_textures.assign(normalMaps.begin(), normalMaps.end());
        
        return newmesh;
    }

    
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; 
                    break;
                }
            }
            if (!skip)
            {   
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }
        return textures;
    }
};


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    LOG_INF("Texture filepath: " << filename.c_str() << " widht: " << width << " height: " << height << " Count of components: " << nrComponents);

    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        LOG_ERR("Texture failed to load at path: " << path);
        stbi_image_free(data);
    }

    return textureID;
}
#endif