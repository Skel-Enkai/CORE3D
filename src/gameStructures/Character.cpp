#include "gs/Character.h"
#include "fs/Camera.h"

#include <GLFW/glfw3.h>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <string>

Character::Character(GLFWwindow *window, std::string modelPath) : Model(modelPath), CharacterCamera(window) {}

void Character::Update(GLFWwindow *window, float FOVdeg, float nearPlane, float farPlane)
{
  CharacterCamera.Inputs(window);
  CharacterCamera.setMatrices(FOVdeg, nearPlane, farPlane);
  position = CharacterCamera.Position + positionOffset;

  float w = glm::dot(CharacterCamera.initialOrientation, CharacterCamera.Orientation);
  // Cross product is not valid for parallel vectors, so these if statements handle this specific cases.
  if (w >= 1.0f)
  {
    rotation = rotationOffset;
  }
  else if (w <= -1.0f)
  {
    rotation = glm::quat(0.0, 1.0, 0.0, 0.0) * rotationOffset;
  }
  else
  {
    // Cross Product cares about order of operation!
    glm::vec3 xyz = glm::cross(CharacterCamera.initialOrientation, CharacterCamera.Orientation);
    glm::quat rot;
    rot.x = xyz.x;
    rot.y = xyz.y;
    rot.z = xyz.z;
    rot.w = w;
    rotation = glm::normalize(rot * rotationOffset);
  }
}

void Character::Draw(Shader &shader)
{
  shader.Activate();
  shader.setFloat("time", glfwGetTime());
  SetTextures(shader);
  for (unsigned int i = 0; i < meshes.size(); i++)
    meshes[i].Mesh::Draw(shader,
                         CharacterCamera,
                         matricesMeshes[i],
                         translationsMeshes[i] + position,
                         rotationsMeshes[i] * rotation,
                         scalesMeshes[i] * scale);
}
