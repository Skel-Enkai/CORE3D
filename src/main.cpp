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
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Character.h"

const unsigned short width = 2000;
const unsigned short height = 1200;

const unsigned short cameraWidth = 1200;
const unsigned short cameraHeight = 1200;

float rectangleVertices[] = 
{
  //  Coords    //  TexCoords
   1.0f, -1.0f,    1.0f, 0.0f,
  -1.0f,  1.0f,    0.0f, 1.0f, 
  -1.0f, -1.0f,    0.0f, 0.0f,

   1.0f,  1.0f,    1.0f, 1.0f, 
  -1.0f,  1.0f,    0.0f, 1.0f,
   1.0f, -1.0f,    1.0f, 0.0f 
};

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
  Shader frameProgram((shaderPath + "framebuffer.vert").c_str(), (shaderPath + "texture.frag").c_str());

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
  frameProgram.Activate();
  glUniform1i(glGetUniformLocation(frameProgram.ID, "screenTexture"), GL_TEXTURE0);

  // Faces are identified as front or back via their clockwise or counter-clockwise indicy rotation.
  // Most games use a counter-clockwise standard for Front Faces, hence glFrontFace(GL_CCW): CCW=CounterClockWise
  /*glCullFace(GL_BACK);*/
  /*glFrontFace(GL_CCW);*/

	std::string modelPath = "resources/models/";
  Model mirror((modelPath + "mirror_blender/mirror.gltf").c_str(), glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.0, 0.0, 0.0));
  Model grassground((modelPath + "grassground/scene.gltf").c_str());
  Model grass((modelPath + "grass/scene.gltf").c_str());

  // Set View from the Mirror
  Camera mirrorView(window, glm::vec3(0.566034, 4.024786, -7.489196));
  mirrorView.Orientation = glm::normalize(glm::vec3(-0.193900, 0.180638, 0.964242));
  mirrorView.updateMatrix(45.0f, 0.1f, 100.0f);

  Character player(window, (modelPath + "crow/scene.gltf").c_str());
  player.CharacterModel.scale = glm::vec3(0.4, 0.4, 0.4);
  player.positionOffset = glm::vec3(1.0, -5.0, 1.5);
  player.rotationOffset = glm::angleAxis(180.0f, glm::vec3(0.0, 1.0, 0.0));
  player.CharacterCamera.updateMatrix(45.0f, 0.1f, 100.0f);

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

  // Create Rectangle for FrameBuffer to Draw to.
  unsigned int rectVAO, rectVBO;
  glGenVertexArrays(1, &rectVAO);
  glGenBuffers(1, &rectVBO);
  glBindVertexArray(rectVAO);
  glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  bool inputMirror = false;

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

      /*std::cout << glm::to_string(mirrorView.Position) << glm::to_string(mirrorView.Orientation) << std::endl;*/

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

    if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
    {
      inputMirror = !inputMirror;
    }
    if (inputMirror)
    {
		  mirrorView.Inputs(window);
		  mirrorView.updateMatrix(45.0f, 0.1f, 100.0f);
    }
    else
    {
      player.Update(window, 45.0f, 0.1f, 100.0f);
    }

    grassground.Draw(shaderProgram, mirrorView);
    grass.Draw(grassProgram, mirrorView);
    player.Draw(shaderProgram, mirrorView);
   
    // Unbind the Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  grassground.Draw(shaderProgram, player.CharacterCamera);
	  grass.Draw(grassProgram, player.CharacterCamera);
    /*player.Draw(shaderProgram);*/
    mirror.Draw(shaderProgram, mirrorProgram, frameBufferTexture, player.CharacterCamera);

    /*glDisable(GL_DEPTH_TEST);*/
    /**/
    /*glActiveTexture(GL_TEXTURE0);*/
    /*glBindTexture(GL_TEXTURE_2D, frameBufferTexture);*/
    /*glBindVertexArray(rectVAO);*/
    /**/
    /*frameProgram.Activate();*/
    /*glDrawArrays(GL_TRIANGLES, 0, 6);*/

    glfwSwapBuffers(window);
    // Take care of all GLFW events
    glfwPollEvents();
  }
  
  shaderProgram.Delete();
  mirrorProgram.Delete();
  grassProgram.Delete();
  frameProgram.Delete();
  // Delete window before ending the program
  glfwDestroyWindow(window);
  // Terminate GLFW before ending the program
  glfwTerminate();
  return 0;
}
