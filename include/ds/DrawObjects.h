#ifndef DRAW_OBJECT_STRUCTS
#define DRAW_OBJECT_STRUCTS

#include <glad/glad.h>

#include "fs/Model.h"
#include "fs/Shader.h"

struct DrawObject
{
  Model &model;
  Shader &shader;
};

#endif
