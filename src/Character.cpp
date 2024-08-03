#include "Character.h"
#include "Camera.h"
#include "GLFW/glfw3.h"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "shaderClass.h"

Character::Character(GLFWwindow* window, const char* modelPath) : CharacterModel(modelPath), CharacterCamera(window) {}

void Character::Update(GLFWwindow* window, float FOVdeg, float nearPlane, float farPlane)
{
  CharacterCamera.Inputs(window);
  CharacterCamera.updateMatrix(FOVdeg, nearPlane, farPlane);
  CharacterModel.position = CharacterCamera.Position + positionOffset;

  float w = glm::dot(CharacterCamera.initialOrientation, CharacterCamera.Orientation);
  // Cross product is not valid for parallel vectors, so these if statements handle this specific cases.
  if (w >= 1.0f)
  {
    CharacterModel.rotation = rotationOffset;
  }
  else if (w <= -1.0f)
  {
    CharacterModel.rotation = glm::quat(0.0, 1.0, 0.0, 0.0) * rotationOffset;
  }
  else
  {
    // Cross Product cares about order of operation!
    glm::vec3 xyz = glm::cross(CharacterCamera.initialOrientation, CharacterCamera.Orientation);
    glm::quat rotation;
    rotation.x = xyz.x;
    rotation.y = xyz.y;
    rotation.z = xyz.z;
    rotation.w = w;
    CharacterModel.rotation = glm::normalize(rotation * rotationOffset);
  }
 }

void Character::Draw(Shader& shader)
{
  CharacterModel.Draw(shader, CharacterCamera);
}

void Character::Draw(Shader& shader, Camera& camera)
{
  CharacterModel.Draw(shader, camera);
}
