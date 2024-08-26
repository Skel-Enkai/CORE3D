#include <cstdlib>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Camera.h"
#include "Character.h"
#include "Skybox.h"
#include "PostProcessFrameBuffer.h"

const unsigned short width = 2000;
const unsigned short height = 1200;

const unsigned short cameraWidth = 1200;
const unsigned short cameraHeight = 1200;

const unsigned short antiAliasingSamples = 8;

std::string modelPath = "resources/models/";
std::string shaderPath = "resources/shaders/";
std::string cubemapsPath = "resources/cubemaps/";

// returns float between -1.0f and 1.0f
float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

// returns float between 0.0f and 1.0f
float randfPositive()
{
  return (static_cast<float>(rand()) /(static_cast<float>(RAND_MAX)));
}

void runSceneOne(GLFWwindow* window, PostProcessingFrameBuffer postProcessor)
{
  Shader shaderProgram((shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "default.frag"));
  Shader playerProgram((shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "player.frag"));
  Shader mirrorProgram((shaderPath + "mirror.vert"), (shaderPath + "post-processing/anti-aliasing-mirror.frag"));
  Shader grassProgram((shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "grass.frag"));
  Shader skyboxProgram((shaderPath + "skybox/skybox.vert"), (shaderPath + "skybox/skybox.frag"));

  Shader reflectionProgram((shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "skybox/reflection.frag"));
  Shader refractionProgram((shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "skybox/refraction.frag"));
  Shader explosionProgram((shaderPath + "default.vert"), (shaderPath + "effects/explosion.geom"), (shaderPath + "default.frag"));
  // Debugging Shader
  Shader normalsProgram((shaderPath + "default.vert"), (shaderPath + "debugging/normals.geom"), (shaderPath + "debugging/normals.frag"));

	// Generate light colour and position
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 22.0f, 5.0f);

  // Set Initial Shader Uniforms
	shaderProgram.Activate();
  shaderProgram.setVec4("lightColor", lightColor);
  shaderProgram.setVec3("lightPos", lightPos);

	playerProgram.Activate();
  playerProgram.setVec4("lightColor", lightColor);
  playerProgram.setVec3("lightPos", lightPos);

  mirrorProgram.Activate();
  mirrorProgram.setInt("multiSampler0", 99);
  mirrorProgram.setInt("texSamples", antiAliasingSamples);

  grassProgram.Activate();
  grassProgram.setVec4("lightColor", lightColor);
  grassProgram.setVec3("lightPos", lightPos);

  skyboxProgram.Activate();
  skyboxProgram.setInt("skybox", 100);

  reflectionProgram.Activate();
  reflectionProgram.setInt("skybox", 100);

  refractionProgram.Activate();
  refractionProgram.setInt("skybox", 100);

  explosionProgram.Activate();
  explosionProgram.setVec4("lightColor", lightColor);
  explosionProgram.setVec3("lightPos", lightPos);

  // Faces are identified as front or back via their clockwise or counter-clockwise indicy rotation.
  // Most games use a counter-clockwise standard for Front Faces, hence glFrontFace(GL_CCW): CCW=CounterClockWise
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearColor(0.12f, 0.10f, 0.21f, 1.0f);

  Model mirror(modelPath + "mirror_blender/mirror.gltf", 1, {}, {}, glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.0, 0.0, 0.0));
  Model grassground(modelPath + "grassground/scene.gltf");
  Model grass(modelPath + "grass/scene.gltf");
  Model ship(modelPath + "spaceship/scene.gltf", 1, {}, {}, glm::vec3(1, 1, 1), glm::vec3(8.0, 5, 10.0),
      glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0)));
  Model statue(modelPath + "statue/scene.gltf", 1, {}, {}, glm::vec3(8, 8, 8), glm::vec3(-5, 10.0, 10.0));

  // Set View from the Mirror
  Camera mirrorView(window, glm::vec3(2.102342, 4.342332, -6.394495));
  mirrorView.Orientation = glm::normalize(glm::vec3(-0.312417, 0.201481, 0.928328));
  mirrorView.updateMatrix(40.0f, 0.1f, 100.0f);

  Character player(window, modelPath + "crow/scene.gltf");
  player.CharacterModel.scale = glm::vec3(0.4, 0.4, 0.4);
  player.positionOffset = glm::vec3(1.0, -5.0, 1.5);
  player.rotationOffset = glm::angleAxis(180.0f, glm::vec3(0.0, 1.0, 0.0));
  player.CharacterCamera.updateMatrix(45.0f, 0.1f, 100.0f);

  // FPS counter
  double prevTime = 0.0;
  unsigned int framesPassed = 0;

  // Mirror Frame Buffer -------------------------------------------------------------------------
  unsigned int mirrorFBO;
  glGenFramebuffers(1, &mirrorFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, mirrorFBO);

  unsigned int frameBufferTexture;
  glGenTextures(1, &frameBufferTexture);
  // GL_TEXTURE_2D_MULTISAMPLE for Anti-Aliasing
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frameBufferTexture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, antiAliasingSamples, GL_RGB16F, cameraWidth, cameraHeight, GL_TRUE);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frameBufferTexture, 0);

  unsigned int mirrorRBO;
  glGenRenderbuffers(1, &mirrorRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, mirrorRBO);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, antiAliasingSamples, GL_DEPTH24_STENCIL8, cameraWidth, cameraHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mirrorRBO);

  auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer error: " << fboStatus << std::endl;
  // --------------------------------------------------------------------------------------------

  std::array<std::string, 6> skyboxPaths =
  {
    cubemapsPath + "sky/right.jpg",
    cubemapsPath + "sky/left.jpg",
    cubemapsPath + "sky/top.jpg",
    cubemapsPath + "sky/bottom.jpg",
    cubemapsPath + "sky/front.jpg",
    cubemapsPath + "sky/back.jpg"
  };
  std::array<std::string, 6> mountainPaths = 
  {
    cubemapsPath + "seamountains/right.jpg",
    cubemapsPath + "seamountains/left.jpg",
    cubemapsPath + "seamountains/top.jpg",
    cubemapsPath + "seamountains/bottom.jpg",
    cubemapsPath + "seamountains/front.jpg",
    cubemapsPath + "seamountains/back.jpg"
  };
  
  SkyBox scenebox(skyboxPaths);

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
      /*std::cout << GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS << std::endl;*/
      /*std::cout << glm::to_string(mirrorView.Position) << glm::to_string(mirrorView.Orientation) << std::endl;*/

      // You could put Input functions in here, as otherwise game time is linked to FPS, however, this causes tearing as slower
      // update than FPS update.
      // Delta time should be used for accurate results.
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mirrorFBO);
    // Clean the back buffer and assign the new colour to it
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
    {
      inputMirror = !inputMirror;
    }
    if (inputMirror)
    {
      mirrorView.Inputs(window);
      mirrorView.updateMatrix(40.0f, 0.1f, 100.0f);
    }
    else
    {
      player.Update(window, 45.0f, 0.1f, 100.0f);
    }

    grassground.Draw(shaderProgram, mirrorView);
    grass.Draw(grassProgram, mirrorView);
    player.Draw(playerProgram, mirrorView);
    ship.Draw(refractionProgram, mirrorView);
    statue.Draw(shaderProgram, mirrorView);
    scenebox.Draw(skyboxProgram, mirrorView);

    // Draw to PostProcessor
    postProcessor.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grassground.Draw(shaderProgram, player.CharacterCamera);
    grass.Draw(grassProgram, player.CharacterCamera);
    mirror.Draw(shaderProgram, mirrorProgram, frameBufferTexture, player.CharacterCamera);
    ship.Draw(explosionProgram, player.CharacterCamera);
    statue.Draw(shaderProgram, player.CharacterCamera);
    // Display Normals for debugging
    statue.Draw(normalsProgram, player.CharacterCamera);
    scenebox.Draw(skyboxProgram, player.CharacterCamera);

    // Unbind and PostProcess the Image
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    postProcessor.Draw();

    glfwSwapBuffers(window);
    // Take care of all GLFW events
    glfwPollEvents();
  }
}

void runSceneTwo(GLFWwindow* window, PostProcessingFrameBuffer postProcessor)
{
  Shader asteroidProgram((shaderPath + "instancing/asteroid.vert"), (shaderPath + "default.geom"), (shaderPath + "default.frag"));
  Shader shaderProgram((shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "default.frag"));
  Shader skyboxProgram((shaderPath + "skybox/skybox.vert"), (shaderPath + "skybox/skybox.frag"));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 22.0f, 5.0f);

  shaderProgram.Activate();
  shaderProgram.setVec4("lightColor", lightColor);
  shaderProgram.setVec3("lightPos", lightPos);

  asteroidProgram.Activate();
  asteroidProgram.setVec4("lightColor", lightColor);
  asteroidProgram.setVec3("lightPos", lightPos);
  
  skyboxProgram.Activate();
  skyboxProgram.setInt("skybox", 100);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glClearColor(0.12f, 0.10f, 0.21f, 1.0f);

  Camera camera(window, glm::vec3(0.0f, 0.0f, 2.0f));

  Model jupiter(modelPath + "jupiter/scene.gltf");

  const unsigned int numAsteroids = 5000;
  float radius = 100.0f;
  float radiusDeviation = 25.0f;

  std::vector <glm::mat4> instanceMatrix;
  std::vector <glm::mat4> rotationMatrix;
  
  for (unsigned int i = 0; i < numAsteroids; i++)
  {
    float x = randf();
    float finalRadius = radius + randf() * radiusDeviation;
    float y = ((rand() % 2) * 2 - 1) * sqrt(1.0f - x * x);

    glm::vec3 tempTranslation;
    glm::quat tempRotation;
    glm::vec3 tempScale;

    if (randf() > 0.5f)
    {
      tempTranslation = glm::vec3(y * finalRadius, randf(), x * finalRadius);
    }
    else
    {
      tempTranslation = glm::vec3(x * finalRadius, randf(), y * finalRadius);
    }
    tempRotation = glm::quat(1.0f, randf(), randf(), randf());
    tempScale = 0.05f * glm::vec3(0.1f + randfPositive(), 0.1f + randfPositive(), 0.1f + randfPositive());

    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    trans = glm::translate(trans, tempTranslation);
    glm::mat4 rot = glm::mat4_cast(tempRotation);
    sca = glm::scale(sca, tempScale);

    instanceMatrix.push_back(trans * sca);
    rotationMatrix.push_back(rot);
  }
  Model asteriod(modelPath + "asteroid/scene.gltf", numAsteroids, instanceMatrix, rotationMatrix);

  std::array<std::string, 6> spacePaths =
  {
    cubemapsPath + "space/right.png",
    cubemapsPath + "space/left.png",
    cubemapsPath + "space/top.png",
    cubemapsPath + "space/bottom.png",
    cubemapsPath + "space/front.png",
    cubemapsPath + "space/back.png"
  };
  SkyBox space(spacePaths);

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
      /*std::cout << GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS << std::endl;*/
      // You could put Input functions in here, as otherwise game time is linked to FPS, however, this causes tearing as slower
      // update than FPS update.
      // Delta time should be used for accurate results.
    }

    postProcessor.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.Inputs(window);
    camera.updateMatrix(45.0f, 0.1f, 1000.0f);

    jupiter.Draw(shaderProgram, camera);
    asteriod.Draw(asteroidProgram, camera);

    space.Draw(skyboxProgram, camera);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    postProcessor.Draw();

    glfwSwapBuffers(window);
    // Take care of all GLFW events
    glfwPollEvents();
  }
}

int main() 
{	
	// Initialise GLFW
	glfwInit();
	
	// Tell GLFW what version of OpenGL were are running
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // Define the number of Samples for anti-aliasing.
  glfwWindowHint(GLFW_SAMPLES, antiAliasingSamples);

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

  
  PostProcessingFrameBuffer postProcessor
    (
      (shaderPath + "post-processing/framebuffer.vert"), 
      (shaderPath + "post-processing/post-processor.frag"),
      101,
      width,
      height
    );

  runSceneOne(window, postProcessor);
  // Delete window before ending the program
  glfwDestroyWindow(window);
  // Terminate GLFW before ending the program
  glfwTerminate();
  return 0;
}
