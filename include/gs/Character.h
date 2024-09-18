#ifndef CHARACTER_CLASS_H
#define CHARACTER_CLASS_H

#include "fs/Model.h"
#include "fs/Shader.h"

#include <GLFW/glfw3.h>
#include <string>

class Character : public Model
{
public:
  Camera CharacterCamera;
  Character(GLFWwindow *window, std::string modelPath);
  void Draw(Shader &shader);
  void Update(GLFWwindow *window, float FOVdeg, float nearPlane, float farPlane);

  glm::vec3 positionOffset = glm::vec3(0.0, 0.0, 0.0);
  glm::quat rotationOffset = glm::quat(1.0, 0.0, 0.0, 0.0);
};

#endif
