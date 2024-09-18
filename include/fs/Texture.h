#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <libraries/stb/stb_image.h>
#include <string>

#include "fs/Shader.h"

class Texture
{
public:
  GLuint ID;
  std::string type;
  GLint unit;

  Texture(const char *image, std::string texType, GLint slot);

  void texUnit(Shader &shader, const char *uniform);
  void Bind();
  void Unbind();
  void Delete();
};

#endif
