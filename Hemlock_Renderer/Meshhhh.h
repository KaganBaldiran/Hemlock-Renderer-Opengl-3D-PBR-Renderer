#pragma once
#ifndef MESH_H
#define MESH_H

#include <glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Camera.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4
namespace newwww {
    struct Vertex {
        
        glm::vec3 Position;
        
        glm::vec3 Normal;
       
        glm::vec2 TexCoords;
       
        glm::vec3 Tangent;
       
        glm::vec3 Bitangent;
        
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        
        float m_Weights[MAX_BONE_INFLUENCE];
    };

    struct Texture {
        unsigned int id = NULL;
        string type;
        string path;
    };

    class Mesh {
    public:
        
        vector<Vertex>       vertices;
        vector<unsigned int> indices;
        vector<Texture>      textures;

        vector<Texture>  diffuse_textures;
        vector<Texture>  specular_textures;
        vector<Texture>  normal_textures;

        

        std::string meshname = "";

        int disableclaymaterial[4] = { 0,0,0,0};

        unsigned int VAO;

        
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
        {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            
            setupMesh();
        }

        void CleanBuffer()
        {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            
            for (auto temp : textures)
            {
                glDeleteTextures(1, &temp.id);
            }

        }

        
        void Draw(GLuint shader, Camera& camera , GLuint shadowMap,GLuint cube_map_texture)
        {

            

            
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;
            unsigned int metalicNr = 1;

            //std::cout << "Texture size: " << textures.size() << "\n";
            for (unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); 
                
                string number;
                string name = textures[i].type;
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++); // transfer unsigned int to string
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++); // transfer unsigned int to string
                else if (name == "texture_height")
                    number = std::to_string(heightNr++); // transfer unsigned int to string
                else if (name == "texture_metalic")
                    number = std::to_string(metalicNr++); // transfer unsigned int to string



                //std::cout << "TEXTURE UNIFORM: " << (name + number).c_str() << "\n";

                
                glUniform1i(glGetUniformLocation(shader, (name + number).c_str()), i);



                if (std::strcmp((name + number).data(), "texture_diffuse1") == 0)
                {

                    disableclaymaterial[0] = 1;
                }
                else if(std::strcmp((name + number).data(), "texture_specular1") == 0)
                {

                    disableclaymaterial[1] = 1;
                }
                else if (std::strcmp((name + number).data(), "texture_normal1") == 0)
                {

                    disableclaymaterial[2] = 1;
                }
                else if (std::strcmp((name + number).data(), "texture_metalic1") == 0)
                {

                    disableclaymaterial[3] = 1;
                }


                GLuint boolArrayLocation = glGetUniformLocation(shader, "disableclaymaterial");

                glUniform1iv(boolArrayLocation, 4, disableclaymaterial);


              
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }

            
            glUniform3f(glGetUniformLocation(shader, "campos"), camera.Position.x, camera.Position.y, camera.Position.z);

            camera.Matrix(shader, "cameramatrix");

            
            //glActiveTexture(GL_TEXTURE0 + 7);

            //glBindTexture(GL_TEXTURE_2D, shadowMap);

            //glUniform1i(glGetUniformLocation(shader, "shadowMap"), 7);

            //glActiveTexture(GL_TEXTURE0 + 10);

            //glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_texture);
            //glUniform1i(glGetUniformLocation(shader, "skybox"), 10);

            // draw mesh
            glBindVertexArray(VAO);
            glUniform1i(glGetUniformLocation(shader, "RenderStep"), 1);

            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glBindTexture(GL_TEXTURE_2D, 0);
            
            glActiveTexture(GL_TEXTURE0);

            
            for (size_t h = 0; h < 4; h++)
            {
               disableclaymaterial[h] = 0;

            }

            GLuint boolArrayLocation = glGetUniformLocation(shader, "disableclaymaterial");

            glUniform1iv(boolArrayLocation, 4, disableclaymaterial);
            
        }

        
        void setupMesh()
        {
            
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

           
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
           
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
          
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
          
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
            
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        
            glEnableVertexAttribArray(5);
            glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

           
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

    private:
        
        unsigned int VBO, EBO;

        
    };
}
#endif