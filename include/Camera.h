#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "glm/fwd.hpp"
#include"shaderClass.h"

class Camera
{
	public:
		glm::vec3 Position;
		glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 initialOrientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
		
    glm::mat4 cameraMatrix = glm::mat4(1.0f);
    glm::mat4 skyboxMatrix = glm::mat4(1.0f);

		bool firstClick = true;

		int Width;
		int Height;

		float speed;
    float slowspeed = 0.06f;
    float fastspeed = 0.25f;
		float sensitivity = 20.0f;

		Camera(GLFWwindow* window, glm::vec3 position = glm::vec3(0.0, 0.0, 0.0));
		
		// Updates the camera matrix to the Vertex Shader
		void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
		// Exports the camera matrix to a shader
		void Matrix(Shader& shader, const char* uniform);
    void SkyboxMatrix(Shader& shader, const char* uniform);
		// Handles camera Inputs
		void Inputs(GLFWwindow* window);
};
#endif
