#include "gs/Mirror.h"
#include "GlobalConstants.h"
#include "fs/Shader.h"
#include "fs/Skybox.h"

#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <vector>

Mirror::Mirror(std::string file,
               GLFWwindow *window,
               unsigned int width,
               unsigned int height,
               GLushort tex,
               glm::vec3 scale,
               glm::vec3 translation,
               glm::quat rotation,
               float near,
               float far,
               GLushort antiAliasingSampleNum)
  : Model(file, scale, translation, rotation), mirrorView(window, width, height, translation),
    mirrorShader(shaderPath + "mirror.vert", shaderPath + "post-processing/anti-aliasing-mirror.frag")
{
  texUnit = tex;
  Width = width;
  Height = height;
  antiAliasingSamples = antiAliasingSampleNum;

  mirrorView.updatePlane(near, far);

  mirrorShader.Activate();
  mirrorShader.setInt("multiSampler0", texUnit);
  mirrorShader.setInt("texSamples", antiAliasingSamples);

  // Calculate average Normal of Mirror and four points on Mirror.
  for (unsigned int j = 0; j < meshes.size(); j++)
  {
    if (meshes[j].name == "mirror")
    {
      glm::vec3 averageVec = glm::vec3(0, 0, 0);
      Mesh mirrorMesh = meshes[j];
      for (unsigned int i = 0; i < mirrorMesh.vertices.size(); i++)
      {
        Vertex vertex = mirrorMesh.vertices[i];
        averageVec += vertex.normal;
        middle += vertex.position;
        if (vertex.position.y > top.y)
          top = vertex.position;
        else if (vertex.position.y < bottom.y)
          bottom = vertex.position;
        if (vertex.position.x > right.x)
          right = vertex.position;
        else if (vertex.position.x < left.x)
          left = vertex.position;
      }
      averageVec /= mirrorMesh.vertices.size();
      middle /= mirrorMesh.vertices.size();
      reflectionNormal = glm::normalize(averageVec);
    }
    mirrorMatrix = matricesMeshes[j];
    meshIndex = j;
  }

  // Reflection Matrix Setup
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), translationsMeshes[meshIndex] + position);
  glm::mat4 rot = glm::mat4_cast(rotationsMeshes[meshIndex] * rotation);
  glm::mat4 sca = glm::scale(glm::mat4(1.0f), scalesMeshes[meshIndex] * scale);
  glm::mat4 worldMatrix = mirrorMatrix * trans * sca * rot;
  glm::vec3 anchorPoint = glm::vec3(worldMatrix * glm::vec4(middle, 1.0));

  // https://ami.uni-eszterhazy.hu/uploads/papers/finalpdf/AMI_40_from175to186.pdf
  reflectionMatrix[0] = glm::vec4(1 - 2 * pow(reflectionNormal.x, 2),
                                  -2 * reflectionNormal.x * reflectionNormal.y,
                                  -2 * reflectionNormal.x * reflectionNormal.z,
                                  0);
  reflectionMatrix[1] = glm::vec4(-2 * reflectionNormal.x * reflectionNormal.y,
                                  1 - 2 * pow(reflectionNormal.y, 2),
                                  -2 * reflectionNormal.y * reflectionNormal.z,
                                  0);
  reflectionMatrix[2] = glm::vec4(-2 * reflectionNormal.x * reflectionNormal.z,
                                  -2 * reflectionNormal.y * reflectionNormal.z,
                                  1 - 2 * pow(reflectionNormal.z, 2),
                                  0);
  float D =
    reflectionNormal.x * anchorPoint.x + reflectionNormal.y * anchorPoint.y + reflectionNormal.z * anchorPoint.z;
  reflectionMatrix[3] =
    glm::vec4(2 * D * reflectionNormal.x, 2 * D * reflectionNormal.y, 2 * D * reflectionNormal.z, 1.0);

  // Temp
  mirrorView.Orientation = reflectionNormal;
  mirrorView.Position = glm::vec3(0.0, 8.3, -6.38);
  mirrorView.updatePlane(near, far);

  // Create Mirror Framebuffer
  glGenFramebuffers(1, &mirrorFBO);
  glGenTextures(1, &frameBufferTexture);

  glBindFramebuffer(GL_FRAMEBUFFER, mirrorFBO);
  glActiveTexture(GL_TEXTURE0 + texUnit);
  // GL_TEXTURE_2D_MULTISAMPLE for Anti-Aliasing
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frameBufferTexture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, antiAliasingSamples, GL_RGB16F, Width, Height, GL_TRUE);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frameBufferTexture, 0);

  glGenRenderbuffers(1, &mirrorRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, mirrorRBO);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, antiAliasingSamples, GL_DEPTH24_STENCIL8, Width, Height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mirrorRBO);

  auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE) std::cout << "Framebuffer error: " << fboStatus << std::endl;
}

void Mirror::Bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, mirrorFBO);
  glViewport(0, 0, Width, Height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Mirror::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void Mirror::UpdateMirror(Camera &camera)
{
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), translationsMeshes[meshIndex] + position);
  glm::mat4 rot = glm::mat4_cast(rotationsMeshes[meshIndex] * rotation);
  glm::mat4 sca = glm::scale(glm::mat4(1.0f), scalesMeshes[meshIndex] * scale);
  glm::mat4 worldMatrix = mirrorMatrix * trans * sca * rot;

  glm::vec3 yTop = glm::normalize(glm::vec3(worldMatrix * glm::vec4(top, 1.0)) - camera.Position);
  glm::vec3 yBottom = glm::normalize(glm::vec3(worldMatrix * glm::vec4(bottom, 1.0)) - camera.Position);
  glm::vec3 xLeft = glm::normalize(glm::vec3(worldMatrix * glm::vec4(left, 1.0)) - camera.Position);
  glm::vec3 xRight = glm::normalize(glm::vec3(worldMatrix * glm::vec4(right, 1.0)) - camera.Position);

  GLfloat FOVx = glm::acos(glm::dot(xLeft, xRight));
  GLfloat FOVy = glm::acos(glm::dot(yTop, yBottom));

  mirrorView.updateXYfov(FOVx, FOVy);
  glm::vec3 position = glm::vec3(reflectionMatrix * glm::vec4(camera.Position, 1.0));
  glm::vec3 center = glm::vec3(worldMatrix * glm::vec4(middle, 1.0));
  mirrorView.updateView(position, center - position);
  mirrorView.updateMatrix();
}

void Mirror::DrawToMirror(std::vector<DrawObject> DrawList, SkyBox skybox)
{
  Bind();
  for (unsigned int i = 0; i < DrawList.size(); i++)
  {
    DrawList[i].model.Draw(DrawList[i].shader, mirrorView);
  }
  skybox.Draw(mirrorView);
  Unbind();
}

void Mirror::Draw(Shader &shader, Camera &camera)
{
  shader.Activate();
  shader.setFloat("time", glfwGetTime());
  SetTextures(shader);
  for (unsigned int i = 0; i < meshes.size(); i++)
    meshes[i].Mesh::Draw(shader,
                         mirrorShader,
                         texUnit,
                         camera,
                         matricesMeshes[i],
                         translationsMeshes[i] + position,
                         rotationsMeshes[i] * rotation,
                         scalesMeshes[i] * scale);
}
