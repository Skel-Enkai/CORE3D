#include "fs/Skybox.h"

#include <iostream>
#include <libraries/stb/stb_image.h>

#include "GlobalConstants.h"
#include "ds/EBO.h"
#include "ds/VAO.h"
#include "fs/Camera.h"

SkyBox::SkyBox(const std::array<std::string, 6> &arr, GLushort texU, std::string vertexFile, std::string fragFile)
  : skyboxShader(shaderPath + vertexFile, shaderPath + fragFile)
{
  texUnit = texU;
  skyboxVAO.Bind();

  // Just creating these objects sets the VAO data to their contents.
  VBO skyboxVBO(skyboxVertices, sizeof(skyboxVertices));
  EBO skyboxEBO(skyboxIndices, sizeof(skyboxIndices));

  skyboxVAO.LinkAttrib(skyboxVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void *)0);

  skyboxVAO.Unbind();
  skyboxVBO.Unbind();
  skyboxEBO.Unbind();

  std::copy(arr.begin(), arr.end(), pathsList.begin());

  glGenTextures(1, &textureID);
  glActiveTexture(GL_TEXTURE0 + texUnit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  for (unsigned int i = 0; i < 6; i++)
  {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(pathsList[i].c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
      stbi_set_flip_vertically_on_load(false);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    }
    else
    {
      std::cout << "Failed to load texture: " << pathsList[i] << std::endl;
      stbi_image_free(data);
    }
  }
  skyboxShader.Activate();
  skyboxShader.setInt("skybox", texUnit);
}

void SkyBox::Draw(Camera &camera)
{
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_FALSE);
  skyboxShader.Activate();
  camera.setSkyboxMatrix(skyboxShader, "camMatrix");

  skyboxVAO.Bind();
  glActiveTexture(GL_TEXTURE0 + texUnit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  skyboxVAO.Unbind();

  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
}

void SkyBox::Draw(Shader &shader, Camera &camera)
{
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_FALSE);
  shader.Activate();
  camera.setSkyboxMatrix(shader, "camMatrix");

  skyboxVAO.Bind();
  glActiveTexture(GL_TEXTURE0 + texUnit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  skyboxVAO.Unbind();

  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
}
