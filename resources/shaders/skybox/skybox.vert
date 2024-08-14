#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 texCoord;

uniform mat4 camMatrix;

void main()
{
  vec4 pos = camMatrix * vec4(aPos, 1.0);
  // The Z value is used as the depth, in the normalized space 1.0 is the maximum value, 
  // making sure the cubemap always has maximum depth.
  // (As in normalized space, the Z axis is always the axis that aligns with the camera)
  gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
  // Cubemaps use a left-handed coordinate system.
  texCoord = vec3(aPos.x, aPos.y, -aPos.z);
}
