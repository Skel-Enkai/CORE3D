#ifndef SHADOW_MAP_CLASS_H
#define SHADOW_MAP_CLASS_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#include "Character.h"
#include "GlobalConstants.h"
#include "Model.h"
#include "shaderClass.h"

class ShadowMap
{
public:
  unsigned int shadowMapTex;
  ShadowMap(unsigned int width,
            unsigned int height,
            unsigned int glTexUnitP,
            glm::vec3 eyePos,
            glm::vec3 direction = glm::vec3(0.0, -1.0, 0.0),
            std::string shadowVertex = shaderPath + "shadowmaps/shadowMap.vert");
  void Bind();
  void Unbind(unsigned int viewWidth, unsigned int viewHeight);
  void DrawToMap(Model &model);
  void DrawToMap(Character &character);
  void AttachMap(std::vector<Shader> shaders);

private:
  unsigned int shadowMapFBO, glTexUnit, mapWidth, mapHeight;
  glm::mat4 orthogonalProjection = glm::ortho(-26.0f, 26.0f, -26.0f, 26.0f, 0.1f, 100.0f);
  glm::mat4 lightView, lightProjection;
  Shader shadowProgram;
};

#endif
