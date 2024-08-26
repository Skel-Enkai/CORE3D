#ifndef CHARACTER_CLASS_H
#define CHARACTER_CLASS_H

#include "Model.h"

#include <GLFW/glfw3.h>
#include <string>

class Character
{
  public:
    Camera CharacterCamera;
    Model CharacterModel;
		Character(GLFWwindow* window, std::string modelPath);
    void Draw(Shader& shader);
    void Draw(Shader& shader, Camera& camera);
    void Update(GLFWwindow* window, float FOVdeg, float nearPlane, float farPlane);
    
    glm::vec3 positionOffset = glm::vec3(0.0, 0.0, 0.0);
    glm::quat rotationOffset = glm::quat(1.0, 0.0, 0.0, 0.0);

};

#endif 
