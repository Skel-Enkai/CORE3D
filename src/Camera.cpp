#include"Camera.h"
#include <GLFW/glfw3.h>
#include <glm/ext/quaternion_geometric.hpp>


Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	// Forms a view matrix from a Position(Eye), a Direction(Center) and an Orientation(Which way is Up)
  glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
	// Forms the perspective matrix with a FOV, a canvas size, nearPlan and farPlane 
  glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), (float)(width/height), nearPlane, farPlane);

	// Sets new camera matrix
	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	// Sets uniform in the Vertex shader, so GPU has access to the final transformation of these two matrices 
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
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
		speed = 0.04f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.01f;
	}
	
	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides Cursor when left mouse button is held down
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		
		// Prevents camera from jumping on the first click
		if (firstClick) 
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}
		
		// Stores the coordinates of the cursor 
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor 
		glfwGetCursorPos(window, &mouseX, &mouseY);
		
		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;
		
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
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
}
