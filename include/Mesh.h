#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include "Camera.h"
#include "VAO.h"

#include <glm/fwd.hpp>
#include <vector>

class Mesh
{
public:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::string name;
  unsigned int instancing;

  VAO mVAO;

  Mesh(std::vector<Vertex> &vertices,
       std::vector<GLuint> &indices,
       std::string name,
       unsigned int instancing = 1,
       std::vector<glm::mat4> instanceMatrix = {},
       std::vector<glm::mat4> rotationMatrix = {});

  void Draw(Shader &shader,
            Camera &camera,
            glm::mat4 matrix = glm::mat4(1.0f),
            glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

  void Draw(Shader &shader,
            Shader &secondaryShader,
            unsigned int mirrorTexture,
            Camera &camera,
            glm::mat4 matrix = glm::mat4(1.0f),
            glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
};
#endif
