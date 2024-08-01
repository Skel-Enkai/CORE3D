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

#include"Model.h"

const unsigned short width = 2000;
const unsigned short height = 1200;

const unsigned short cameraWidth = 1200;
const unsigned short cameraHeight = 1200;

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
	
	// Specify the viewport of OpenGL in the Window 
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

	std::string shaderPath = "resources/shaders/";
  Shader shaderProgram((shaderPath + "default.vert").c_str(), (shaderPath + "default.frag").c_str());
  Shader mirrorProgram((shaderPath + "default.vert").c_str(), (shaderPath + "texture.frag").c_str());
  Shader grassProgram((shaderPath + "default.vert").c_str(), (shaderPath + "grass.frag").c_str());

	// Generate light colour and position
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 22.0f, 5.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
  mirrorProgram.Activate();
  glUniform1i(glGetUniformLocation(mirrorProgram.ID, "screenTexture"), GL_TEXTURE0);
	glUniform4f(glGetUniformLocation(mirrorProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(mirrorProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
  grassProgram.Activate();
	glUniform4f(glGetUniformLocation(grassProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(grassProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

  // Faces are identified as front or back via their clockwise or counter-clockwise indicy rotation.
  // Most games use a counter-clockwise standard for Front Faces, hence glFrontFace(GL_CCW): CCW=CounterClockWise
  /*glCullFace(GL_BACK);*/
  /*glFrontFace(GL_CCW);*/

	// Initialises Camera Object with resolution and position 
	Camera camera(width, height, glm::vec3(4.0f, 5.0f, 10.0f));

	std::string modelPath = "resources/models/";
  Model mirror((modelPath + "mirror_blender/mirror.gltf").c_str(), glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.0, 0.0, 0.0));
  Model grassground((modelPath + "grassground/scene.gltf").c_str());
  Model grass((modelPath + "grass/scene.gltf").c_str());
  Model crow((modelPath + "crow/scene.gltf").c_str(), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.0, 0.0, 15.0));

  // Set View from the Mirror
  Camera mirrorView(cameraWidth, cameraHeight, glm::vec3(0, 5.0, -5.0));
  mirrorView.Orientation = glm::normalize(glm::vec3(0.0, -0.05, 0.8));
  mirrorView.updateMatrix(45.0f, 0.1f, 100.0f);
  camera.updateMatrix(45.0f, 0.1f, 100.0f);

   // FPS counter
  double prevTime = 0.0;
  unsigned int framesPassed = 0;

  // Frame buffer
  unsigned int FBO;
  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  unsigned int frameBufferTexture;
  glGenTextures(1, &frameBufferTexture);
  glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cameraWidth, cameraHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);

  unsigned int RBO;
  glGenRenderbuffers(1, &RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, cameraWidth, cameraHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

  auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer error: " << fboStatus << std::endl;

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
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		// Specify the colour of the background
		glClearColor(0.12f, 0.10f, 0.21f, 1.0f);
		// Clean the back buffer and assign the new colour to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

		/*mirrorView.Inputs(window);*/
		/* mirrorView.updateMatrix(45.0f, 0.1f, 100.0f);*/

    grassground.Draw(shaderProgram, mirrorView);
    grass.Draw(grassProgram, mirrorView);
    crow.Draw(shaderProgram, mirrorView);
   
    // Unbind the Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.Inputs(window);
    camera.updateMatrix(45.0f, 0.1f, 100.0f);

	  grassground.Draw(shaderProgram, camera);
    grass.Draw(grassProgram, camera);
    crow.Draw(shaderProgram, camera);
    mirror.Draw(shaderProgram, mirrorProgram, frameBufferTexture, camera);
   
    glfwSwapBuffers(window);
    // Take care of all GLFW events
    glfwPollEvents();
  }
  
  shaderProgram.Delete();
  mirrorProgram.Delete();
  grassProgram.Delete();
  // Delete window before ending the program
  glfwDestroyWindow(window);
  // Terminate GLFW before ending the program
  glfwTerminate();
  return 0;
}
