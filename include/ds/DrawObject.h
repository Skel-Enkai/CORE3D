#ifndef DRAW_OBJECT
#define DRAW_OBJECT

#include <glad/glad.h>

#include "fs/Model.h"
#include "fs/Shader.h"

struct DrawObject
{
  Model &model;
  Shader &shader;
};

#endif
