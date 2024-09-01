#include <iostream>

#include "PostProcessFrameBuffer.h"
#include "VAO.h"
#include "shaderClass.h"

PostProcessingFrameBuffer::PostProcessingFrameBuffer(std::string vertexFile,
                                                     std::string fragmentFile,
                                                     unsigned int glTextureU,
                                                     unsigned int width,
                                                     unsigned int height,
                                                     unsigned short antiAliasingSamples,
                                                     float gammaCorrection)
  : postProcessingShader(vertexFile, fragmentFile)
{
  glTextureUnit = glTextureU;

  rectVAO.Bind();
  VBO rectVBO(rectangleVertices, sizeof(rectangleVertices));
  rectVAO.LinkAttrib(rectVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), (void *)0);
  rectVAO.LinkAttrib(rectVBO, 1, 2, GL_FLOAT, 4 * sizeof(float), (void *)(2 * sizeof(float)));
  rectVBO.Unbind();
  rectVAO.Unbind();

  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  glGenTextures(1, &frameBufferTexture);
  glActiveTexture(GL_TEXTURE0 + glTextureUnit);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frameBufferTexture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, antiAliasingSamples, GL_RGB16F, width, height, GL_TRUE);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frameBufferTexture, 0);

  glGenRenderbuffers(1, &RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, antiAliasingSamples, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

  auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE) std::cout << "Framebuffer error: " << fboStatus << std::endl;

  postProcessingShader.Activate();
  postProcessingShader.setInt("multiSampler0", glTextureUnit);
  postProcessingShader.setInt("texSamples", antiAliasingSamples);
  postProcessingShader.setFloat("gamma", gammaCorrection);
}

void PostProcessingFrameBuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void PostProcessingFrameBuffer::Bind() { glBindFramebuffer(GL_FRAMEBUFFER, FBO); }

void PostProcessingFrameBuffer::Draw()
{
  glActiveTexture(GL_TEXTURE0 + glTextureUnit);
  glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

  glDisable(GL_DEPTH_TEST);
  rectVAO.Bind();

  postProcessingShader.Activate();
  glDrawArrays(GL_TRIANGLES, 0, 6);

  rectVAO.Unbind();
  glEnable(GL_DEPTH_TEST);
}

void PostProcessingFrameBuffer::DrawTexture(unsigned int texUnit)
{
  glActiveTexture(GL_TEXTURE0 + glTextureUnit);
  glBindTexture(GL_TEXTURE_2D, texUnit);

  glDisable(GL_DEPTH_TEST);
  rectVAO.Bind();

  postProcessingShader.Activate();
  postProcessingShader.setInt("screenTexture", glTextureUnit);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  rectVAO.Unbind();
  glEnable(GL_DEPTH_TEST);
}
