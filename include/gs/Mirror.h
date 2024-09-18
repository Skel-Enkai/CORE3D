#ifndef MIRROR_CLASS_H
#define MIRROR_CLASS_H

#include <glad/glad.h>

#include "ds/DrawObjects.h"
#include "fs/Camera.h"
#include "fs/Model.h"
#include "fs/Skybox.h"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <vector>

class Mirror : public Model
{
public:
  Mirror(std::string file,
         GLFWwindow *window,
         unsigned int width,
         unsigned int height,
         GLushort tex,
         glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0),
         glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0),
         glm::quat rotation = glm::quat(1.0, 0.0, 0.0, 0.0),
         float near = 0.1f,
         float far = 100.0f,
         GLushort antiAliasingSampleNum = 8);

  GLushort texUnit;

  void Draw(Shader &shader, Camera &camera);
  void Bind();
  void Unbind();
  void UpdateMirror(Camera &camera);
  void DrawToMirror(std::vector<DrawObject> DrawList, SkyBox skybox);

private:
  Camera mirrorView;
  Shader mirrorShader;
  glm::vec3 reflectionNormal;
  glm::vec3 bottom = glm::vec3(0.0, 0.0, 0.0);
  glm::vec3 top = glm::vec3(0.0, 0.0, 0.0);
  glm::vec3 right = glm::vec3(0.0, 0.0, 0.0);
  glm::vec3 left = glm::vec3(0.0, 0.0, 0.0);
  glm::vec3 middle = glm::vec3(0.0, 0.0, 0.0);
  GLuint mirrorFBO, frameBufferTexture, mirrorRBO;
  GLushort antiAliasingSamples;
  unsigned int Width, Height, meshIndex;
  glm::mat4 mirrorMatrix;
  glm::mat4 reflectionMatrix = glm::mat4(1.0f);
};
#endif
