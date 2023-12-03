#include "NormalBaker.hpp"


std::pair<GLuint,int> BAKER::BakeNormal( GLuint NormalCubeMapSize, Model* highPolyModel, Model* lowPolyModel, GLuint HighPolyShader , GLuint LowPolyShader , Camera& camera)
{
    GLuint fbo, rbo, NormalCubeMap;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, NormalCubeMapSize, NormalCubeMapSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Frame buffer error :: " << status << "\n";
        return { 0,-1 };
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenTextures(1, &NormalCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, NormalCubeMap);

    for (size_t i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, NormalCubeMapSize, NormalCubeMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
       // glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            //0, GL_RGB, NormalCubeMapSize, NormalCubeMapSize, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);



    GLuint lowfbo, lowrbo, NormalMap;
    glGenFramebuffers(1, &lowfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, lowfbo);

    glGenRenderbuffers(1, &lowrbo);
    glBindRenderbuffer(GL_RENDERBUFFER, lowrbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, NormalCubeMapSize, NormalCubeMapSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, lowrbo);

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Frame buffer error :: " << status << "\n";
        return { 0,-1 };
    }

 
    glGenTextures(1, &NormalMap);
    glBindTexture(GL_TEXTURE_2D, NormalMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, NormalCubeMapSize, NormalCubeMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
        
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NormalMap, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    UseShaderProgram(HighPolyShader);
    glViewport(0, 0, NormalCubeMapSize, NormalCubeMapSize);

    //glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, -0.1f, 100.0f);
    glm::mat4 captureProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 10.0f);

   /* glm::mat4 fboViews[] =
    {
       glm::lookAt(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };*/

    glm::mat4 fboViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    for (size_t i = 0; i < 6; i++)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, NormalCubeMap, 0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        auto ShaderPrep = [&]() {
            glUniformMatrix4fv(glGetUniformLocation(HighPolyShader, "proj"), 1, GL_FALSE, glm::value_ptr(captureProjection));
            glUniformMatrix4fv(glGetUniformLocation(HighPolyShader, "view"), 1, GL_FALSE, glm::value_ptr(fboViews[i]));
        };

        highPolyModel->transformation.SendUniformToShader(HighPolyShader, "model");
        highPolyModel->Draw(HighPolyShader, ShaderPrep);
    }

    /*
    GLuint quadVAO, quadVBO;

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };


    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    */


    UseShaderProgram(0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDeleteRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glDeleteFramebuffers(1, &lowfbo);
    glDeleteRenderbuffers(1, &lowrbo);
    //glDeleteVertexArrays(1, &quadVAO);
    //glDeleteBuffers(1, &quadVBO);

    return { NormalCubeMap,1 };
}
