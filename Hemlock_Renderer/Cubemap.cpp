#include "Cubemap.h"
#include "stb_image.h"
#include "Texture.h"
#include "Log.h"


CubeMap::CubeMap(std::vector<std::string> texture_faces ,const char* vertex_source, const char* frag_source)
{
    this->texture_faces.assign(texture_faces.begin(),texture_faces.end());
	
	glGenTextures(1, &this->cubemaptextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubemaptextureID);

    int width, height, nrchannels;
    unsigned char* data;

    for (size_t i = 0; i < texture_faces.size(); i++)
    {
        data = stbi_load(this->texture_faces[i].c_str(), &width, &height, &nrchannels, 0);

        if (data)
        {

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);

        }
        else
        {

            std::cout << "Cubemap error :: failed to load at path  " << this->texture_faces.at(i) << "\n";
            stbi_image_free(data);

        }
    }


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    cubemapshader = new Shader(vertex_source, frag_source);


    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };



    vao.Bind();
    vbo.Bind();

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),(void*)0);

    BindVAONull();
    BindVBONull();


}

CubeMap::CubeMap(GLuint CubeMap, const char* vertex_source, const char* frag_source)
{

    this->cubemaptextureID = CubeMap;
    cubemapshader = new Shader(vertex_source, frag_source);

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };



    vao.Bind();
    vbo.Bind();

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    BindVAONull();
    BindVBONull();
}

CubeMap::~CubeMap()
{
    delete this->cubemapshader;
    glDeleteTextures(1, &this->cubemaptextureID);
}

void CubeMap::Draw(Camera &camera, Vec2<float> windowSize)
{
    
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    UseShaderProgram(cubemapshader->GetID());

    glm::mat4 view = glm::mat4(glm::mat3(camera.cam_view));

    glUniform1i(glGetUniformLocation(cubemapshader->GetID(), "skybox"), 0);
    glUniformMatrix4fv(glGetUniformLocation(cubemapshader->GetID(), "projection"), 1, GL_FALSE, glm::value_ptr(camera.projection));
    glUniformMatrix4fv(glGetUniformLocation(cubemapshader->GetID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemapshader->GetID(), "ScreenRatio"), 1, GL_FALSE, glm::value_ptr(camera.screenratiodefault));

    vao.Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubemaptextureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    BindVAONull();

    UseShaderProgram(0);

}

void CubeMap::SetCubeMapTexture(GLuint& CubeMapTexture)
{
    glDeleteTextures(1, &this->cubemaptextureID);
    this->cubemaptextureID = CubeMapTexture;
}

std::pair<GLuint, int> HDRItoCubeMap(const char* HDRI, unsigned int CubeMapSize, GLuint HDRItoCubeMapShader)
{
    int width, height, components;

    stbi_set_flip_vertically_on_load(true);
    float *data = stbi_loadf(HDRI, &width, &height, &components, 0);
    
    GLuint HDRItexture;

    if (data)
    {
           GLenum Format = 0;

           if (components == 1)
           {
              Format = GL_RED;
           }
           else if (components == 4)
           {
             Format = GL_RGBA;
           }
           else if (components == 3)
           {
             Format = GL_RGB;
           }
           else
           {
               LOG_ERR("Cannot load the HDRI texture!");
               return { 0,HDRI_INCOMPATIBLE_FILE };
           }

            glGenTextures(1, &HDRItexture);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, HDRItexture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, Format, GL_FLOAT, data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);

            glBindTexture(GL_TEXTURE_2D, 0); 

    }
    else
    {
        LOG_ERR("Cannot load the HDRI texture!");
        return { 0,HDRI_INCOMPATIBLE_FILE };
    }


    GLuint fbo , rbo , CubeMapColorAttachment;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CubeMapSize, CubeMapSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERR("Error completing the framebuffer!");
        return { 0,HDRI_ERROR };
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenTextures(1, &CubeMapColorAttachment);
    glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapColorAttachment);

    for (size_t i = 0; i < 6; i++)
    {

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, CubeMapSize, CubeMapSize, 0, GL_RGB, GL_FLOAT, nullptr);

    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    
    float skyboxVertices[] = {
                 
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    VBO vbo;
    VAO vao;

    vao.Bind();
    vbo.Bind();

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    BindVBONull();
    BindVAONull();

    glm::mat4 Projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

    glm::mat4 fboViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    
    glUseProgram(HDRItoCubeMapShader);
    glUniformMatrix4fv(glGetUniformLocation(HDRItoCubeMapShader, "ProjectionMat"), 1, GL_FALSE, glm::value_ptr(Projection));

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, HDRItexture);
    glUniform1i(glGetUniformLocation(HDRItoCubeMapShader, "hdriTexture"), 0);

    glViewport(0,0, CubeMapSize, CubeMapSize);
    vao.Bind();

    for (size_t i = 0; i < 6; i++)
    {

        glUniformMatrix4fv(glGetUniformLocation(HDRItoCubeMapShader, "ViewMat"), 1, GL_FALSE, glm::value_ptr(fboViews[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, CubeMapColorAttachment, 0);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    BindVAONull();
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);

    glDeleteRenderbuffers(1 ,&rbo);
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &HDRItexture);

    return { CubeMapColorAttachment,HDRI_COMPLETE };
}
