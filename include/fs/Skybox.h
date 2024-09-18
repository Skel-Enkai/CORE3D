#ifndef SKYBOX_CLASS_H
#define SKYBOX_CLASS_H

#include <array>
#include <string>

#include "fs/Camera.h"
#include "ds/VAO.h"
#include "fs/Shader.h"

class SkyBox
{
public:
  SkyBox(const std::array<std::string, 6> &arr, 
         GLushort texUnit = 100, 
         std::string vertFile = "skybox/skybox.vert", 
         std::string fragFile = "skybox/skybox.frag");
  void Draw(Camera &camera);
  void Draw(Shader &shader, Camera &camera);

private:
  float skyboxVertices[24] = {
    //   Coordinates
    -1.0f, -1.0f,  1.0f, //        7--------6
     1.0f, -1.0f,  1.0f, //       /|       /|
     1.0f, -1.0f, -1.0f, //      4--------5 |
    -1.0f, -1.0f, -1.0f, //      | |      | |
    -1.0f,  1.0f,  1.0f, //      | 3------|-2
     1.0f,  1.0f,  1.0f, //      |/       |/
     1.0f,  1.0f, -1.0f, //      0--------1
    -1.0f,  1.0f, -1.0f};

  unsigned int skyboxIndices[36] = {
    // Right
    6, 2, 1,
    1, 5, 6,
    // Left
    7, 4, 0,
    0, 3, 7,
    // Top
    6, 5, 4,
    4, 7, 6,
    // Bottom
    2, 3, 0,
    0, 1, 2,
    // Back
    5, 1, 0,
    0, 4, 5,
    // Front
    6, 7, 3,
    3, 2, 6};

  std::array<std::string, 6> pathsList;
  VAO skyboxVAO;
  unsigned int textureID;
  GLushort texUnit;

  Shader skyboxShader;
};

#endif
