#include "fs/Mesh.h"

#include "ds/EBO.h"
#include "ds/VAO.h"
#include "glm/fwd.hpp"
#include <string>

Mesh::Mesh(std::vector<Vertex> &vertices,
           std::vector<GLuint> &indices,
           std::string name,
           unsigned int instancing,
           std::vector<glm::mat4> instanceMatrix,
           std::vector<glm::mat4> rotationMatrix)
{
  Mesh::vertices = vertices;
  Mesh::indices = indices;
  Mesh::name = name;

  Mesh::instancing = instancing;

  mVAO.Bind();

  VBO instanceVBO(instanceMatrix);
  VBO rotationVBO(rotationMatrix);
  VBO mVBO(vertices);
  EBO mEBO(indices);
  // Links VBO attributes to the mVAO
  mVAO.LinkAttrib(mVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
  mVAO.LinkAttrib(mVBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(3 * sizeof(float)));
  mVAO.LinkAttrib(mVBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)(6 * sizeof(float)));
  mVAO.LinkAttrib(mVBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)(9 * sizeof(float)));
  if (instancing != 1)
  {
    instanceVBO.Bind();

    mVAO.LinkAttrib(instanceVBO, 4, 4, GL_FLOAT, sizeof(glm::mat4), (void *)0);
    mVAO.LinkAttrib(instanceVBO, 5, 4, GL_FLOAT, sizeof(glm::mat4), (void *)(1 * sizeof(glm::vec4)));
    mVAO.LinkAttrib(instanceVBO, 6, 4, GL_FLOAT, sizeof(glm::mat4), (void *)(2 * sizeof(glm::vec4)));
    mVAO.LinkAttrib(instanceVBO, 7, 4, GL_FLOAT, sizeof(glm::mat4), (void *)(3 * sizeof(glm::vec4)));

    mVAO.LinkAttrib(rotationVBO, 8, 4, GL_FLOAT, sizeof(glm::mat4), (void *)0);
    mVAO.LinkAttrib(rotationVBO, 9, 4, GL_FLOAT, sizeof(glm::mat4), (void *)(1 * sizeof(glm::vec4)));
    mVAO.LinkAttrib(rotationVBO, 10, 4, GL_FLOAT, sizeof(glm::mat4), (void *)(2 * sizeof(glm::vec4)));
    mVAO.LinkAttrib(rotationVBO, 11, 4, GL_FLOAT, sizeof(glm::mat4), (void *)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);

    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);
    glVertexAttribDivisor(11, 1);
  }
  // Unbinds to prevent accidental modification
  mVAO.Unbind();
  mVBO.Unbind();
  mEBO.Unbind();
  instanceVBO.Unbind();
  rotationVBO.Unbind();
}

void Mesh::Draw(
  Shader &shader, glm::mat4 matrix, glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
{
  mVAO.Bind();

  if (instancing == 1)
  {
    // forms matrices
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 rot = glm::mat4_cast(rotation);
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

    // sends matrices to the Shader
    shader.setMat4("translation", trans);
    shader.setMat4("rotation", rot);
    shader.setMat4("scale", sca);
    shader.setMat4("initMatrix", matrix);

    // Draw the actual Mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  }
  else
  {
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instancing);
  }
}

void Mesh::Draw(
  Shader &shader, Camera &camera, glm::mat4 matrix, glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
{
  mVAO.Bind();

  // Take care of the Camera Matrix
  shader.setVec3("camPos", camera.Position);
  camera.setCamMatrix(shader, "camMatrix");

  if (instancing == 1)
  {
    // forms matrices
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 rot = glm::mat4_cast(rotation);
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

    // sends matrices to the Shader
    shader.setMat4("translation", trans);
    shader.setMat4("rotation", rot);
    shader.setMat4("scale", sca);
    shader.setMat4("initMatrix", matrix);

    // Draw the actual Mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  }
  else
  {
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instancing);
  }
}

void Mesh::Draw(Shader &primaryShader,
                Shader &secondaryShader,
                unsigned int mirrorTexture,
                Camera &camera,
                glm::mat4 matrix,
                glm::vec3 translation,
                glm::quat rotation,
                glm::vec3 scale)
{
  mVAO.Bind();
  // forms matrices
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
  glm::mat4 rot = glm::mat4_cast(rotation);
  glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

  if (Mesh::name == "mirror")
  {
    secondaryShader.Activate();

    glActiveTexture(GL_TEXTURE0 + 99);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mirrorTexture);
    // Take care of the Camera Matrix
    secondaryShader.setVec3("camPos", camera.Position);
    camera.setCamMatrix(secondaryShader, "camMatrix");
    // sends matrices to the secondaryShader
    if (instancing == 1)
    {
      secondaryShader.setMat4("translation", trans);
      secondaryShader.setMat4("rotation", rot);
      secondaryShader.setMat4("scale", sca);
      secondaryShader.setMat4("initMatrix", matrix);
    }
  }

  else
  {
    primaryShader.Activate();

    // Take care of the Camera Matrix
    primaryShader.setVec3("camPos", camera.Position);
    camera.setCamMatrix(primaryShader, "camMatrix");
    // sends matrices to the primaryShader
    if (instancing == 1)
    {
      primaryShader.setMat4("translation", trans);
      primaryShader.setMat4("rotation", rot);
      primaryShader.setMat4("scale", sca);
      primaryShader.setMat4("initMatrix", matrix);
    }
  }

  if (instancing == 1)
  {
    // Draw the actual Mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  }
  else
  {
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instancing);
  }
}
