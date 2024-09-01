#ifndef POST_PROCESSING_FB_CLASS_H
#define POST_PROCESSING_FB_CLASS_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "VAO.h"
#include "shaderClass.h"

class PostProcessingFrameBuffer
{
public:
  PostProcessingFrameBuffer(std::string fragmentFile,
                            std::string vertexFile,
                            unsigned int glTextureUnit,
                            unsigned int width,
                            unsigned int height,
                            unsigned short antiAliasingSamples = 8,
                            float gammaCorrection = 2.2);
  void Bind();
  void Unbind();
  void Draw();
  void DrawTexture(unsigned int texUnit);

private:
  float rectangleVertices[24] = {
    //  Coords    //  TexCoords
     1.0f, -1.0f,     1.0f, 0.0f, 
    -1.0f,  1.0f,     0.0f, 1.0f, 
    -1.0f, -1.0f,     0.0f, 0.0f,

     1.0f,  1.0f,     1.0f, 1.0f, 
    -1.0f,  1.0f,     0.0f, 1.0f, 
     1.0f, -1.0f,     1.0f, 0.0f};

  unsigned int RBO, FBO, frameBufferTexture, glTextureUnit;
  VAO rectVAO;
  Shader postProcessingShader;
};

#endif
