#include "Camera.h"
#include <vector>
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_impl_glfw.h"

Vec2<double> ScrollAmount;
Vec2<double> MousePosCamera;

std::vector<Camera*> Cameras;
int ActiveCameraID = 0;
int CameraIterator = 0;
GLuint Camvbo, Camvao, Camebo;
unsigned int IndicesCount;

Camera::Camera(int window_width, int window_height, glm::vec3 position)
{
	this->CameraID = CameraIterator;

	w_width = window_width;
	w_height = window_height;
	Position = position;

	Objectview = glm::mat4(1.0f);
	targetPosition = glm::vec3(0.0f);
	CameraIterator++;
}

Camera::~Camera()
{
	
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane , GLFWwindow* window, Vec2<int> menu_size, bool TakeScreenShot)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);

	this->cam_view = view;

	int height = NULL, width = NULL;
	glfwGetWindowSize(window, &width, &height);

	this->nearPlane = nearPlane;
	this->farPlane = farPlane;

	float aspect_rat = 1.0f;
	if (TakeScreenShot)
	{
		aspect_rat = (float)width / (float)height;
	}
	
	//float aspect_rat = (float)menu_size.x / (float)menu_size.y;

	if (ActiveCameraID == this->CameraID)
	{
		Objectview = glm::lookAt(glm::vec3(0.0f), -Orientation, Up);
		Objectview = glm::translate(Objectview, -Position);
	}

	proj = glm::perspective(glm::radians(FOVdeg),aspect_rat , nearPlane, farPlane);
	this->projection = proj;

	screenratiodefault = glm::mat4(1.0f);
	screenratiodefault = glm::scale(screenratiodefault, glm::vec3(GetScreenRatio(window,menu_size).x, GetScreenRatio(window,menu_size).y, 1.0f));
	
	if (TakeScreenShot)
	{
		cammatrix = proj * view;
		screenratiodefault = glm::mat4(1.0f);
	}
	else
	{
		cammatrix = screenratiodefault * proj * view;
	}
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane, Vec2<int> WindowSize)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);

	this->cam_view = view;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	float aspect_rat = (float)WindowSize.x / (float)WindowSize.y;

	if (ActiveCameraID == this->CameraID)
	{
		Objectview = glm::lookAt(glm::vec3(0.0f), -Orientation, Up);
		Objectview = glm::translate(Objectview, -Position);
	}

	proj = glm::perspective(glm::radians(FOVdeg), aspect_rat, nearPlane, farPlane);
	this->projection = proj;
	cammatrix = proj * view;
}

void Camera::Matrix(GLuint shaderprogram, const char* uniform)
{
	
	glUniformMatrix4fv(glGetUniformLocation(shaderprogram, uniform), 1, GL_FALSE, glm::value_ptr(cammatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "proj"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "view"), 1, GL_FALSE, glm::value_ptr(cam_view));

}

void Camera::HandleInputs(GLFWwindow* window, Vec2<int> menu_size , Vec2<int> WindowSize, int cameraLayout)
{


	if (cameraLayout == CAMERA_LAYOUT_FIRST_PERSON)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{

			Position += speed * Orientation;

		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{

			Position += speed * -glm::normalize(glm::cross(Orientation, Up));

		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{

			Position += speed * -Orientation;

		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{

			Position += speed * glm::normalize(glm::cross(Orientation, Up));;

		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{

			Position += speed * Up;

		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{

			Position += speed * -Up;

		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{

			speed = 0.07f;

		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		{

			speed = 0.03f;

		}


		if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS))
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			if (firstclick)
			{
				glfwSetCursorPos(window, (WindowSize.x / 2), (WindowSize.y / 2));
				firstclick = false;
			}

			Vec2<double> mousepos;

			glfwGetCursorPos(window, &mousepos.x, &mousepos.y);

			Vec2<float> rot;

			rot.x = sensitivity * (float)(mousepos.y - (WindowSize.y / 2)) / WindowSize.y;
			rot.y = sensitivity * (float)(mousepos.x - (WindowSize.x / 2)) / WindowSize.x;

			glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rot.x), glm::normalize(glm::cross(Orientation, Up)));

			if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
				Orientation = newOrientation;
			}

			Orientation = glm::rotate(Orientation, glm::radians(-rot.y), Up);
			glfwSetCursorPos(window, (WindowSize.x / 2), (WindowSize.y / 2));
		}

		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstclick = true;
		}
	}
	else if (cameraLayout == CAMERA_LAYOUT_INDUSTRY_STANDARD)
	{
		float CameraSensitivity = 10.0f;
		speed = 0.5;
		Vec2<double> CurrentMousePos;
		glfwGetCursorPos(window, &CurrentMousePos.x, &CurrentMousePos.y);

		Vec2<double> deltaMouse(CurrentMousePos - MousePosCamera);

		if (ScrollAmount.y == 1)
		{
			Position += speed * Orientation;
		}
		if (ScrollAmount.y == -1)
		{
			Position += speed * -Orientation;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			Position += (float)deltaMouse.y * (speed * 0.5f) * Orientation;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
		{
			glm::vec3 PositionVecLength(this->Position.length());

			if (deltaMouse.x > 0)
			{
				glm::vec3 deltaPosition(PositionVecLength* glm::vec3(deltaMouse.x / WindowSize.x) * -glm::normalize(glm::cross(Orientation, Up)));
				Position += deltaPosition;
				targetPosition += deltaPosition;
			}
			if (deltaMouse.x < 0)
			{
				glm::vec3 deltaPosition(PositionVecLength* glm::vec3(-deltaMouse.x / WindowSize.x)* glm::normalize(glm::cross(Orientation, Up)));
				Position += deltaPosition;
			    targetPosition += deltaPosition;
			}
			if (deltaMouse.y < 0)
			{
				glm::vec3 deltaPosition(PositionVecLength* glm::vec3(-deltaMouse.y / WindowSize.y) * -Up);
				Position += deltaPosition;
				targetPosition += deltaPosition;
			}
			if (deltaMouse.y > 0)
			{
				glm::vec3 deltaPosition(PositionVecLength* glm::vec3(deltaMouse.y / WindowSize.y)* Up);
				Position += deltaPosition;
				targetPosition += deltaPosition;
			}
		}

		
		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS))
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			if (firstclick)
			{
				glfwSetCursorPos(window, (WindowSize.x / 2), (WindowSize.y / 2));
				firstclick = false;
			}

			Vec2<double> mousepos;
			glfwGetCursorPos(window, &mousepos.x, &mousepos.y);

			Vec2<float> rot;

			rot.x = CameraSensitivity * (float)(mousepos.y - (WindowSize.y / 2)) / WindowSize.y;
			rot.y = CameraSensitivity * (float)(mousepos.x - (WindowSize.x / 2)) / WindowSize.x;

			glm::vec3 cameraToTarget = targetPosition - Position;
			float distanceToTarget = glm::length(cameraToTarget);
			glm::quat rotation = glm::quat(glm::vec3(-rot.x, -rot.y, 0.0f));
			cameraToTarget = glm::rotate(rotation, cameraToTarget);
			Position = targetPosition - distanceToTarget * glm::normalize(cameraToTarget);
			Orientation = glm::normalize(targetPosition - Position);

			glfwSetCursorPos(window, (WindowSize.x / 2), (WindowSize.y / 2));
		}

		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstclick = true;
		}

		MousePosCamera(CurrentMousePos);
	}


	ScrollAmount({ 0,0 });

}

void Camera::Draw(glm::mat4& cammatrix, Shader& shader, std::function<void()> shaderPrep)
{
	shader.use();
	glBindVertexArray(Camvao);

	shader.setMat4("cameramatrix",cammatrix);
	shader.setMat4("model", glm::inverse(Objectview));

	shader.setVec4("lightColor", glm::vec4(255/255.0f, 69 / 255.0f, 0, 1.0f));

	glDrawElements(GL_LINES, IndicesCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	UseShaderProgram(0);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ScrollAmount({ xoffset, yoffset });
}

void InitializeCameraMesh()
{
	std::vector<float> vertices = {
		0.5f, 0.4f, 0.35f,
		0.5f, -0.6f, 0.35f,
		-0.5f, 0.4f, 0.35f,
		-0.5f, -0.6f, 0.35f,

		0.25f, 0.15f, 0.0f,
		0.25f, -0.35f, 0.0f,
		-0.25f, 0.15f, 0.0f,
		-0.25f, -0.35f, 0.0f,

		0.0f, 0.6f, 0.0f,
		0.0f, -0.1f, -0.35f
	};

	std::vector<unsigned int> indices = {
		0,1,1,3,3,2,2,0,
		4,5,5,7,7,6,6,4,
		0,4,1,5,2,6,3,7,
		4,8,6,8,
		4,9,5,9,6,9,7,9
	};

	IndicesCount = indices.size();

	glGenVertexArrays(1, &Camvao);
	glGenBuffers(1, &Camvbo);

	glBindVertexArray(Camvao);
	glBindBuffer(GL_ARRAY_BUFFER, Camvbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glGenBuffers(1, &Camebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Camebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DisposeCameraMesh()
{
	glDeleteBuffers(1, &Camvbo);
	glDeleteBuffers(1, &Camebo);
	glDeleteVertexArrays(1, &Camvao);
	LOG_INF("Camera mesh disposed!");
}

int AddCamera(glm::vec3 Position , glm::vec3 Orientation)
{
	Camera* temp = new Camera(1000, 1000, Position);
	temp->Orientation = Orientation;
	temp->Objectview = glm::lookAt(glm::vec3(0.0f), -temp->Orientation, temp->Up);
	temp->Objectview = glm::translate(temp->Objectview, -temp->Position);
	Cameras.push_back(temp);
	return temp->CameraID;
}

void DisposeCameras()
{
	for (size_t i = 0; i < Cameras.size(); i++)
	{
		delete Cameras[i];
	}
	Cameras.clear();
	CameraIterator = 0;
	LOG_INF("Cameras disposed!");
}

void DisposeAllocatedCameras()
{
	for (size_t i = 1; i < Cameras.size(); i++)
	{
		delete Cameras[i];
		Cameras.erase(Cameras.begin() + i);
	}
	LOG_INF("Cameras disposed!");
}

void DrawCameras(glm::mat4& cammatrix, Shader& shader, std::function<void()> shaderPrep)
{
	for (size_t i = 0; i < Cameras.size(); i++)
	{
		if (i != ActiveCameraID) 
		{
			Cameras[i]->Draw(cammatrix, shader, shaderPrep);
		}
	}
}
