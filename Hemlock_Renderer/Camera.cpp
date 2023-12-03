#include "Camera.h"
#include "VectorMath.h"
#include "Texture.h"

Vec2<double> ScrollAmount;
Vec2<double> MousePosCamera;

Camera::Camera(int window_width, int window_height, glm::vec3 position)
{
	w_width = window_width;
	w_height = window_height;
	Position = position;

	targetPosition = glm::vec3(0.0f);
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane , GLFWwindow* window, Vec2<int> menu_size)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);

	this->cam_view = view;

	int height = NULL, width = NULL;
	glfwGetWindowSize(window, &width, &height);

	this->nearPlane = nearPlane;
	this->farPlane = farPlane;

	//std::cout <<"IN CAMERA: " << "WINSIZE.X: " << width - menu_size.x << "WINSIZE.Y: " << menu_size.y << "\n";

	//float aspect_rat = (float)(width - menu_size.x) / (float)menu_size.y;
	float aspect_rat = (float)(w_width/w_height);
	//float aspect_rat = (float)((1000 - 175.438) / (1000 - 18));

	//std::cout << "ASPECT RATIO: "<< aspect_rat << "\n";


	proj = glm::perspective(glm::radians(FOVdeg),aspect_rat , nearPlane, farPlane);

	this->projection = proj;
	//proj = glm::ortho(0, w_width, w_height, 0, 0, 100);

	screenratiodefault = glm::mat4(1.0f);
	screenratiodefault = glm::scale(screenratiodefault, glm::vec3(GetScreenRatio(window,menu_size).x, GetScreenRatio(window,menu_size).y, 1.0f));
	cammatrix = screenratiodefault * proj * view;
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
			int height = NULL, width = NULL;
			glfwGetWindowSize(window, &width, &height);

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			if (firstclick)
			{
				glfwSetCursorPos(window, (w_width / 2), (w_height / 2));
				firstclick = false;
			}

			Vec2<double> mousepos;

			glfwGetCursorPos(window, &mousepos.x, &mousepos.y);

			Vec2<float> rot;

			rot.x = sensitivity * (float)(mousepos.y - (w_height / 2)) / w_height;
			rot.y = sensitivity * (float)(mousepos.x - (w_width / 2)) / w_width;

			glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rot.x), glm::normalize(glm::cross(Orientation, Up)));

			if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{

				Orientation = newOrientation;

			}

			Orientation = glm::rotate(Orientation, glm::radians(-rot.y), Up);

			//int height = NULL, width = NULL;
			//glfwGetWindowSize(window, &width, &height);

			glfwSetCursorPos(window, (w_width / 2), (w_height / 2));
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
			int height = NULL, width = NULL;
			glfwGetWindowSize(window, &width, &height);

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			if (firstclick)
			{
				glfwSetCursorPos(window, (w_width / 2), (w_height / 2));
				firstclick = false;
			}

			Vec2<double> mousepos;
			glfwGetCursorPos(window, &mousepos.x, &mousepos.y);

			Vec2<float> rot;

			rot.x = CameraSensitivity * (float)(mousepos.y - (w_height / 2)) / w_height;
			rot.y = CameraSensitivity * (float)(mousepos.x - (w_width / 2)) / w_width;

			glm::vec3 cameraToTarget = targetPosition - Position;
			float distanceToTarget = glm::length(cameraToTarget);
			glm::quat rotation = glm::quat(glm::vec3(-rot.x, -rot.y, 0.0f));
			cameraToTarget = glm::rotate(rotation, cameraToTarget);
			Position = targetPosition - distanceToTarget * glm::normalize(cameraToTarget);
			Orientation = glm::normalize(targetPosition - Position);

			glfwSetCursorPos(window, (w_width / 2), (w_height / 2));
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


