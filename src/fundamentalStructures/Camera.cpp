#include"Camera.h"
#include <GLFW/glfw3.h>
#include <glm/ext/quaternion_geometric.hpp>

Camera::Camera(GLFWwindow* window, glm::vec3 position)
{
  glfwGetWindowSize(window, &Width, &Height);
	Position = position;
  initialOrientation = Orientation;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	// Forms a view matrix from a Position(Eye), a Direction(Center) and an Orientation(Which way is Up)
  view = glm::lookAt(Position, Position + Orientation, Up);
	// Forms the perspective matrix with a FOV, a canvas size, nearPlan and farPlane 
  projection = glm::perspective(glm::radians(FOVdeg), (static_cast<float>(Width) / static_cast<float>(Height)), nearPlane, farPlane);
	// Sets new camera matrix
	cameraMatrix = projection * view;
  skyboxMatrix = projection * glm::mat4(glm::mat3(view));
}

void Camera::setCamMatrix(Shader& shader, const char* uniform)
{
  shader.setMat4(uniform, cameraMatrix);
}

void Camera::setViewMatrix(Shader& shader, const char* uniform)
{
  shader.setMat4(uniform, view);
}

void Camera::setSkyboxMatrix(Shader& shader, const char* uniform)
{
  shader.setMat4(uniform, skyboxMatrix);
}

void Camera::Inputs(GLFWwindow* window)
{
	// Up and Orientation are vec3's defined in the header files
	// cross product is used to find the vector that is the normal of the plane made by the two vectors
	// These if statements define the movement and orientation of the camera
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
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
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
		speed = fastspeed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = slowspeed;
	}
	
	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides Cursor when left mouse button is held down
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		
		// Prevents camera from jumping on the first click
		if (firstClick) 
		{
			glfwSetCursorPos(window, (Width / 2), (Height / 2));
			firstClick = false;
		}
		
		// Stores the coordinates of the cursor 
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor 
		glfwGetCursorPos(window, &mouseX, &mouseY);
		
		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (static_cast<float> (Height) / 2)) / static_cast<float>(Height);
		float rotY = sensitivity * (float)(mouseX - (static_cast<float> (Width) / 2)) / static_cast<float>(Width);
		
		// Calculates the upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));
		
		// Decides whether or not the next vertical Orientation is legal or not
		// As when Orientation approaches Up, as the cross product of any two identical vectors is 0.
		//if (!((glm::angle(newOrientation, Up) <= glm::radians(5.0f)) or (glm::angle(newOrientation, -Up) <= glm::radians(5.0f))))
		// Applied De Morgan's Law
		if ((glm::angle(newOrientation, Up) > glm::radians(5.0f)) and (glm::angle(newOrientation, -Up) > glm::radians(5.0f)))
		{
			Orientation = newOrientation;
		}
		
		// Rotates the Orientation left and right (horizontal orientation)
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (Width / 2), (Height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
}
