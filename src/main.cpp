#include <cstdlib>
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
#include <string>
/*#include "glm/ext/quaternion_trigonometric.hpp"*/

#include"Model.h"

const unsigned int width = 2000;
const unsigned int height = 1400;
int mac_width, mac_height;

const unsigned int numWindows = 10;
glm::vec3 positionsWin[numWindows];
float rotationsWin[numWindows];

unsigned int drawOrder[numWindows];
float distanceCamera[numWindows];

// returns random float between 0.0f and 1.0f 
float randomFloat()
{
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

int compareDist(const void* a, const void* b)
{
  float diff = distanceCamera[*(int*)b] - distanceCamera[*(int*)a];
  // returns the farthest one first
  return (diff > 0) - (diff < 0);
}

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
	Shader grassProgram((shaderPath + "default.vert").c_str(), (shaderPath + "grass.frag").c_str());
  Shader windowProgram((shaderPath + "default.vert").c_str(), (shaderPath + "windows.frag").c_str());

	// Generate light colour and position
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	grassProgram.Activate();
	glUniform4f(glGetUniformLocation(grassProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(grassProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Stops behind layers from displaying during animation (Tests for depth continiously)
	glEnable(GL_DEPTH_TEST);
  // Configure face culling
  // Without face culling all faces are drawn in order of the depth, significantly wasting drawing time and fragshader compute time.
  glEnable(GL_CULL_FACE);
  // Faces are identified as front or back via their clockwise or counter-clockwise indicy rotation.
  // Most games use a counter-clockwise standard for Front Faces, hence glFrontFace(GL_CCW): CCW=CounterClockWise
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  // Set blending function.
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialises Camera Object with resolution and position 
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	std::string modelPath = "resources/models/";

  Model ground((modelPath + "grassground/scene.gltf").c_str());
  Model grass((modelPath + "grass/scene.gltf").c_str());
  Model windows((modelPath + "windows/scene.gltf").c_str(), glm::vec3(2.0, 3.0, 2.0), glm::vec3(0.0, -2.0, 0.0));

  for (unsigned int i = 0; i < numWindows; i++)
  {
    positionsWin[i] = glm::vec3
      (
       // Min -10.0f, Max 10.0f
       -10.0f +  (10.0f - (-10.0f)) * randomFloat(), 
       // Min 1.0f, Max 4.0f
       1.0f +  (4.0f - (1.0f)) * randomFloat(),
       -10.0f +  (10.0f - (-10.0f)) * randomFloat() 
      );
    rotationsWin[i] = randomFloat();
    drawOrder[i] = i;
  }

  // FPS counter
  double prevTime = 0.0;
  unsigned int framesPassed = 0;

  // Main while loop 
	while(!glfwWindowShouldClose(window))
  {
    // FPS counter loop
    double currentTime = glfwGetTime();
    double timeDiff = currentTime - prevTime;
    framesPassed++;
    if (timeDiff >= 1.0 / 30.0)
    {
      std::string FPS = std::to_string(framesPassed / timeDiff);
      // Multiply by 1000 to transform to Miliseconds
      std::string ms = std::to_string(timeDiff / framesPassed * 1000);
      std::string newTitle = "CORE3D - " + FPS + "FPS / " + ms + "ms";
      glfwSetWindowTitle(window, newTitle.c_str());
      prevTime = currentTime;
      framesPassed = 0;
      // You could put Input functions in here, as otherwise game time is linked to FPS, however, this causes tearing as slower
      // update than FPS update.
      // Delta time should be used for accurate results.
    }
		// Specify the colour of the background
		glClearColor(0.089f, 0.158f, 0.209f, 1.0f);
		// Clean the back buffer and assign the new colour to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Gets Input to move the camera
		camera.Inputs(window);
		// Calls Matrix function of Camera object to set the Camera view in the shaderProgram
	  camera.updateMatrix(45.0f, 0.1f, 100.0f);
  
    glEnable(GL_CULL_FACE);
    ground.Draw(shaderProgram, camera);
    glDisable(GL_CULL_FACE);
    grass.Draw(grassProgram, camera);

    glEnable(GL_BLEND);
    
    for (unsigned int i = 0; i < numWindows; i++)
    {
      distanceCamera[i] =  glm::length(positionsWin[i] - camera.Position);
    }
    qsort(drawOrder, numWindows, sizeof(unsigned int), compareDist);

    for (unsigned int i = 0; i < numWindows; i++)
    {
      windows.Draw(windowProgram, camera, positionsWin[drawOrder[i]], glm::quat(1.0f, 0.0f, rotationsWin[drawOrder[i]], 0.0f));
    }
    glDisable(GL_BLEND);

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
