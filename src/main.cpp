#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>
#include "glm/ext/quaternion_trigonometric.hpp"

#include"Model.h"

const unsigned int width = 800;
const unsigned int height = 800;
int mac_width, mac_height;

int main() 
{	
	// Initialise GLFW
	glfwInit();
	
	// Tell GLFW what version of OpenGL were are running
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Tell GLFW we are using the CORE profile
	// Core profile only contains modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// Create a GLFWwindow object of width by height pixels, naming it "YoutubeOpenGL"
	GLFWwindow * window = glfwCreateWindow(width, height, "CORE3D", NULL, NULL);
	
	// Error check if the window fails to create
	if (window == NULL)
	{	// pipes error message to cout, and Terminates GLFW
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1; 
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	
	// Load GLAD so it configures OpenGL
	gladLoadGL();
	
	glfwGetFramebufferSize(window, &mac_width, &mac_height);
	// Specify the viewport of OpenGL in the Window 
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, mac_width, mac_height);


	std::string shaderPath = "resources/shaders/";
	Shader shaderProgram((shaderPath + "default.vert").c_str(), (shaderPath + "default.frag").c_str());
	//Shader shaderProgram1((shaderPath + "default.vert").c_str(), (shaderPath + "default.frag").c_str());

	// Generate light colour and position
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Stops behind layers from displaying during animation (Tests for depth continiously)
	glEnable(GL_DEPTH_TEST);

	// Initialises Camera Object with resolution and position 
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	std::string modelPath = "resources/models/";

  glm::quat shipRot = glm::angleAxis(-1.5708f, glm::vec3(1.0, 0.0, 0.0));
	Model spaceship((modelPath + "spaceship/scene.gltf").c_str(), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.0, 0.0, -5.0), shipRot);
  Model bunny((modelPath + "bunny/scene.gltf").c_str(), glm::vec3(15.0, 15.0, 15.0), glm::vec3(0.8, 0.0, -0.5));
  glm::quat swordRot = glm::angleAxis(0.785398f, glm::vec3(0.0, 0.0, 1.0));
  Model sword((modelPath + "sword/scene.gltf").c_str(), glm::vec3(0.05, 0.05, 0.05), glm::vec3(0.0, 0.0, -12.0), swordRot);

	// Main while loop 
	while(!glfwWindowShouldClose(window))
 	{
		// Specify the colour of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new colour to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Gets Input to move the camera
		camera.Inputs(window);
		// Calls Matrix function of Camera object to set the Camera view in the shaderProgram
		camera.updateMatrix(45.0f, 0.1f, 100.0f);
    
    spaceship.Draw(shaderProgram, camera); 
		sword.Draw(shaderProgram, camera);
    bunny.Draw(shaderProgram, camera);

		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);	
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
