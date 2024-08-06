#include "Skybox.h"

#include<stb/stb_image.h>
#include<iostream>

#include "Camera.h"
#include "VAO.h"
#include "EBO.h"

SkyBox::SkyBox(const std::array<std::string, 6>& arr)
{
  skyboxVAO.Bind();

  // Just creating these objects sets the VAO data to their contents.
  VBO skyboxVBO(skyboxVertices, sizeof(skyboxVertices));
  EBO skyboxEBO(skyboxIndices, sizeof(skyboxIndices));

  skyboxVAO.LinkAttrib(skyboxVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

  skyboxVAO.Unbind();
  skyboxVBO.Unbind();
  skyboxEBO.Unbind();

  std::copy(arr.begin(), arr.end(), pathsList.begin());

  glGenTextures(1, &textureID);
  glActiveTexture(GL_TEXTURE0 + 100);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  for (unsigned int i = 0; i < 6; i++)
  {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(pathsList[i].c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
      stbi_set_flip_vertically_on_load(false);
      glTexImage2D
      (
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
        0,
        GL_RGB,
        width, 
        height, 
        0,
        GL_RGB, 
        GL_UNSIGNED_BYTE,
        data
      );
      stbi_image_free(data);
    }
    else
    {
      std::cout << "Failed to load texture: " << pathsList[i] << std::endl;
      stbi_image_free(data);
    }
  }
}

void SkyBox::Draw(Shader& shader, Camera& camera)
{
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_FALSE);
  shader.Activate();
	camera.SkyboxMatrix(shader, "camMatrix");
  
  glBindVertexArray(skyboxVAO.ID);
  glActiveTexture(GL_TEXTURE0 + 100);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
}
