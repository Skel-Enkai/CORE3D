#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <math.h>
#include <vector>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>
#include <libraries/stb/stb_image.h>

#include "GlobalConstants.h"
#include "ds/DrawObjects.h"
#include "fs/Camera.h"
#include "fs/PostProcessFrameBuffer.h"
#include "fs/Shader.h"
#include "fs/ShadowMap.h"
#include "fs/Skybox.h"
#include "gs/Character.h"
#include "gs/Mirror.h"

const unsigned short width = 2000;
const unsigned short height = 1200;

const unsigned short cameraHeight = 1024 * 3;
const unsigned short cameraWidth = 400 * 3;

const GLushort skyboxTex = 100;

const unsigned short antiAliasingSamples = 8;

// returns float between -1.0f and 1.0f
float randf() { return -1.0f + (rand() / (RAND_MAX / 2.0f)); }

// returns float between 0.0f and 1.0f
float randfPositive() { return (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX))); }

void runSceneOne(GLFWwindow *window, PostProcessingFrameBuffer postProcessor)
{
  Shader shaderProgram((shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "default.frag"));
  Shader playerProgram((shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "player.frag"));
  Shader grassProgram((shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "grass.frag"));
  /*Shader reflectionProgram(*/
  /*  (shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "skybox/reflection.frag"));*/
  Shader refractionProgram(
    (shaderPath + "default.vert"), (shaderPath + "default.geom"), (shaderPath + "skybox/refraction.frag"));
  /*Shader explosionProgram(*/
  /*  (shaderPath + "default.vert"), (shaderPath + "effects/explosion.geom"), (shaderPath + "default.frag"));*/
  // Debugging Shader
  Shader normalsProgram((shaderPath + "debugging/normals.vert"),
                        (shaderPath + "debugging/normals.geom"),
                        (shaderPath + "debugging/normals.frag"));

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

  grassProgram.Activate();
  grassProgram.setVec4("lightColor", lightColor);
  grassProgram.setVec3("lightPos", lightPos);

  /*reflectionProgram.Activate();*/
  /*reflectionProgram.setInt("skybox", skyboxTex);*/

  refractionProgram.Activate();
  refractionProgram.setInt("skybox", skyboxTex);

  /*explosionProgram.Activate();*/
  /*explosionProgram.setVec4("lightColor", lightColor);*/
  /*explosionProgram.setVec3("lightPos", lightPos);*/

  // Faces are identified as front or back via their clockwise or
  // counter-clockwise indicy rotation. Most games use a counter-clockwise
  // standard for Front Faces, hence glFrontFace(GL_CCW): CCW=CounterClockWise
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearColor(0.12f, 0.10f, 0.21f, 1.0f);

  Mirror mirror(
    modelPath + "mirror_blender/mirror.gltf", window, cameraWidth, cameraHeight, 99, glm::vec3(0.02, 0.02, 0.02));
  Model grassground(modelPath + "grassground/scene.gltf");
  Model grass(modelPath + "grass/scene.gltf");
  Model ship(modelPath + "spaceship/scene.gltf",
             glm::vec3(1, 1, 1),
             glm::vec3(8.0, 5, 10.0),
             glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0)));
  Model statue(modelPath + "statue/scene.gltf", glm::vec3(8, 8, 8), glm::vec3(-5, 10.0, 10.0));

  /* This approximation of Orientation and FOV of the mirror and only works when the mirror is far away and not at
     an extreme angle. To fix this a new FOV and Orientation would need to be calculated based on the Player's
     position. For realistic results this would need to be calculated when the player and/or mirror moves.
     FOV would increase as the player approaches the mirror.
     Orientation will be along the line of reflection from the players view.*/

  Character player(window, modelPath + "crow/scene.gltf");
  player.scale = glm::vec3(0.4, 0.4, 0.4);
  player.positionOffset = glm::vec3(0, -7.0, 0);
  player.rotationOffset = glm::angleAxis(180.0f, glm::vec3(0.0, 1.0, 0.0));
  player.CharacterCamera.setMatrices(45.0f, 0.1f, 100.0f);

  // FPS counter
  double prevTime = 0.0;
  unsigned int framesPassed = 0;

  std::array<std::string, 6> skyboxPaths = {cubemapsPath + "sky/right.jpg",
                                            cubemapsPath + "sky/left.jpg",
                                            cubemapsPath + "sky/top.jpg",
                                            cubemapsPath + "sky/bottom.jpg",
                                            cubemapsPath + "sky/front.jpg",
                                            cubemapsPath + "sky/back.jpg"};

  std::array<std::string, 6> mountainPaths = {cubemapsPath + "seamountains/right.jpg",
                                              cubemapsPath + "seamountains/left.jpg",
                                              cubemapsPath + "seamountains/top.jpg",
                                              cubemapsPath + "seamountains/bottom.jpg",
                                              cubemapsPath + "seamountains/front.jpg",
                                              cubemapsPath + "seamountains/back.jpg"};

  SkyBox scenebox(skyboxPaths, skyboxTex);
  ShadowMap sceneShadow(4000, 4000, 98, glm::vec3(0.0, 15.0, 0.0));
  std::vector<Shader> shadowShaders = {grassProgram, shaderProgram, playerProgram};
  sceneShadow.AttachMap(shadowShaders);

  /*std::vector<Model> shadowDraw = {player, ship, statue, mirror};*/

  std::vector<DrawObject> mirrorDraw = {DrawObject{player, playerProgram},
                                        DrawObject{grassground, shaderProgram},
                                        DrawObject{grass, grassProgram},
                                        DrawObject{ship, shaderProgram},
                                        DrawObject{statue, shaderProgram}};

  /*std::vector<DrawObject> sceneDraw = {DrawObject{mirror, shaderProgram},*/
  /*                                     DrawObject{grassground, shaderProgram},*/
  /*                                     DrawObject{grass, grassProgram},*/
  /*                                     DrawObject{statue, shaderProgram},*/
  /*                                     DrawObject{ship, shaderProgram}};*/

  // Main while loop
  while (!glfwWindowShouldClose(window))
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

      // You could put Input functions in here, as otherwise game time is linked
      // to FPS, however, this causes tearing as slower update than FPS update.
      // Delta time should be used for accurate results.
    }
    player.Update(window, 45.0f, 0.1f, 100.0f);
    // Draw to ShadowMap
    sceneShadow.Bind();
    sceneShadow.DrawToMap(player);
    sceneShadow.DrawToMap(ship);
    sceneShadow.DrawToMap(statue);
    sceneShadow.DrawToMap(mirror);
    sceneShadow.Unbind();

    mirror.UpdateMirror(player.CharacterCamera);
    mirror.DrawToMirror(mirrorDraw, scenebox);

    // Draw to PostProcessor
    postProcessor.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grassground.Draw(shaderProgram, player.CharacterCamera);
    grass.Draw(grassProgram, player.CharacterCamera);
    mirror.Draw(shaderProgram, player.CharacterCamera);
    ship.Draw(shaderProgram, player.CharacterCamera);
    statue.Draw(shaderProgram, player.CharacterCamera);
    scenebox.Draw(player.CharacterCamera);

    // Unbind and PostProcess the Image
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    postProcessor.Draw();

    glfwSwapBuffers(window);
    // Take care of all GLFW events
    glfwPollEvents();
  }
}

void runSceneTwo(GLFWwindow *window, PostProcessingFrameBuffer postProcessor)
{
  Shader asteroidProgram(
    (shaderPath + "instancing/asteroid.vert"), (shaderPath + "default.geom"), (shaderPath + "default.frag"));
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

  std::vector<glm::mat4> instanceMatrix;
  std::vector<glm::mat4> rotationMatrix;

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

  std::array<std::string, 6> spacePaths = {cubemapsPath + "space/right.png",
                                           cubemapsPath + "space/left.png",
                                           cubemapsPath + "space/top.png",
                                           cubemapsPath + "space/bottom.png",
                                           cubemapsPath + "space/front.png",
                                           cubemapsPath + "space/back.png"};
  SkyBox space(spacePaths);

  // FPS counter
  double prevTime = 0.0;
  unsigned int framesPassed = 0;

  // Main while loop
  while (!glfwWindowShouldClose(window))
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
      // You could put Input functions in here, as otherwise game time is linked
      // to FPS, however, this causes tearing as slower update than FPS update.
      // Delta time should be used for accurate results.
    }

    postProcessor.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.Inputs(window);
    camera.setMatrices(45.0f, 0.1f, 1000.0f);

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

  GLFWwindow *window = glfwCreateWindow(width, height, "CORE3D", NULL, NULL);

  // Error check if the window fails to create
  if (window == NULL)
  { // pipes error message to cout, and Terminates GLFW
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

  PostProcessingFrameBuffer postProcessor((shaderPath + "post-processing/framebuffer.vert"),
                                          (shaderPath + "post-processing/post-processor.frag"),
                                          101,
                                          width,
                                          height);

  runSceneOne(window, postProcessor);
  // Delete window before ending the program
  glfwDestroyWindow(window);
  // Terminate GLFW before ending the program
  glfwTerminate();
  return 0;
}
