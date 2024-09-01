#include "ShadowMap.h"
#include "Character.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>

ShadowMap::ShadowMap(unsigned int width,
                     unsigned int height,
                     unsigned int glTexUnitP,
                     glm::vec3 eyePos,
                     glm::vec3 direction,
                     std::string shaderVertex)
  : shadowProgram(shaderVertex)
{
  glTexUnit = glTexUnitP;
  mapWidth = width;
  mapHeight = height;

  glGenFramebuffers(1, &shadowMapFBO);
  glGenTextures(1, &shadowMapTex);

  glActiveTexture(GL_TEXTURE0 + glTexUnit);
  glBindTexture(GL_TEXTURE_2D, shadowMapTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  // Prevents darkness outside the the Frustrum by setting Maximum Depth.
  // Check using one float is valid for DepthBuffer
  // float clampColour[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float clampColour[] = {1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColour);

  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTex, 0);
  // Needed as we aren't touching the ColorBuffer
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // 3rd Argument UP, cannot align exactly with the view vector formed.
  // Perhaps write some code that assigns UP direction appropriately
  lightView = glm::lookAt(eyePos, eyePos + glm::normalize(direction), glm::vec3(0.0f, 0.0f, 1.0f));
  lightProjection = orthogonalProjection * lightView;

  shadowProgram.Activate();
  shadowProgram.setMat4("lightProjection", lightProjection);
}

void ShadowMap::Bind()
{
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, mapWidth, mapHeight);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::Unbind(unsigned int viewWidth, unsigned int viewHeight)
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, viewWidth, viewHeight);
}

void ShadowMap::DrawToMap(Model &model) { model.DrawShadow(shadowProgram); }

void ShadowMap::DrawToMap(Character &character) { character.DrawShadow(shadowProgram); }

void ShadowMap::AttachMap(std::vector<Shader> shaders)
{
  for (unsigned int i = 0; i < shaders.size(); i++)
  {
    shaders[i].Activate();
    shaders[i].setMat4("lightProjection", lightProjection);
    shaders[i].setInt("shadowMap", glTexUnit);
  }
  glActiveTexture(GL_TEXTURE0 + glTexUnit);
  glBindTexture(GL_TEXTURE_2D, shadowMapTex);
}
