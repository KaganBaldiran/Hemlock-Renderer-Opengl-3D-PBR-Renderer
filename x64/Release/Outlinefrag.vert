#version 330 core

  layout(location = 0) in vec3 vertexdata;

  layout(location = 1) in vec3 aNormal;
  
  uniform mat4 model;
  
  uniform mat4 cameramatrix;
  

  void main()
  {
    vec4 pos = vec4(vertexdata + aNormal * 0.07, 1.0);
    gl_Position = cameramatrix * model * pos;

  }


  