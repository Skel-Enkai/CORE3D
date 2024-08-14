#ifndef SKYBOX_CLASS_H
#define SKYBOX_CLASS_H

#include<glad/glad.h>
#include<string>
#include<array>
#include "Camera.h"
#include"VAO.h"

class SkyBox 
{
  public:
    SkyBox(const std::array<std::string, 6>& arr);
    void Draw(Shader& shader, Camera& camera); 
  private:
    float skyboxVertices[24] =
    {
      //   Coordinates
	    -1.0f, -1.0f,  1.0f,//        7--------6
	     1.0f, -1.0f,  1.0f,//       /|       /|
	     1.0f, -1.0f, -1.0f,//      4--------5 |
	    -1.0f, -1.0f, -1.0f,//      | |      | |
	    -1.0f,  1.0f,  1.0f,//      | 3------|-2
	     1.0f,  1.0f,  1.0f,//      |/       |/
	     1.0f,  1.0f, -1.0f,//      0--------1
	    -1.0f,  1.0f, -1.0f
    };

    unsigned int skyboxIndices[36] =
    {
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
    	3, 2, 6
    };

    std::array<std::string, 6> pathsList;
    VAO skyboxVAO;
    unsigned int textureID;
};


#endif
