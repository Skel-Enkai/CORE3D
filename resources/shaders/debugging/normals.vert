	
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColour;
layout (location = 3) in vec2 aTex;

out DATA 
{
  vec3 Normal;
  vec3 Colour;
  vec2 TexCoord;
  mat4 projection;
} data_out;

// The final view matrix formed by the Camera 
uniform mat4 camMatrix;
// Model matrices
uniform mat4 initMatrix;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
  // Each matrix multiplication is expensive, set Final Matrix in Draw call when possible
  gl_Position = initMatrix * translation * scale * rotation * vec4(aPos, 1.0);
  data_out.Normal = vec3(rotation * vec4(aNormal, 1.0));
  data_out.Colour = aColour;
  data_out.TexCoord = aTex;
  data_out.projection = camMatrix;
}
