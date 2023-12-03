#ifndef  CAMERA
#define CAMERA

#include <glew.h>
#include <glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "Shader.h"

#include "VectorMath.h"
#include "Log.h"

#define CAMERA_LAYOUT_FIRST_PERSON 0X100
#define CAMERA_LAYOUT_INDUSTRY_STANDARD 0X101

extern Vec2<double> ScrollAmount;

  class Camera
  {
  public:

	  glm::vec3 Position;
	  glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	  glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	  glm::mat4 cammatrix = glm::mat4(1.0f);
	  glm::mat4 projection;
	  glm::mat4 cam_view;
	  glm::mat4 screenratiodefault;

	  float nearPlane, farPlane;

	  bool firstclick = true;

	  int w_width;
	  int w_height;

	  float speed = 0.03f;
	  float sensitivity = 100.0f;

	  glm::vec3 targetPosition;


	  Camera(int window_width, int window_height, glm::vec3 position);


	  void updateMatrix(float FOVdeg, float nearPlane, float farPlane, GLFWwindow* window, Vec2<int> menu_size);

	  void Matrix(GLuint shaderprogram, const char* uniform);

	  void HandleInputs(GLFWwindow* window, Vec2<int> menu_size, Vec2<int> WindowSize, int cameraLayout);

	  glm::vec3 Get_Position() { return Position; };

	  glm::vec3 Get_Orientation() { return Orientation; };

	  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) 
	  {
		  ScrollAmount({ xoffset, yoffset });
	  }
  };


#endif 

