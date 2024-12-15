#ifndef MIRROR_OBJECT
#define MIRROR_OBJECT

#include "gs/Mirror.h"
#include "fs/Shader.h"

struct MirrorObject
{
  Mirror &mirror;
  Shader &shader;
};

#endif
