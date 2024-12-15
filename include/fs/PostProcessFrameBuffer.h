#ifndef POST_PROCESSING_FB_CLASS_H
#define POST_PROCESSING_FB_CLASS_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <vector>
#include "ds/DrawObject.h"
#include "ds/MirrorObject.h"
#include "ds/VAO.h"
#include "fs/Shader.h"
#include "fs/Skybox.h"

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
  void DrawToBuffer(Camera &camera, std::vector<DrawObject> drawingList, SkyBox skybox);
  void DrawToBuffer(Camera &camera, std::vector<DrawObject> drawingList, std::vector<MirrorObject> mirrorList, SkyBox skybox);
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

  unsigned int RBO, FBO, frameBufferTexture, glTextureUnit, Width, Height;
  VAO rectVAO;
  Shader postProcessingShader;
};

#endif
