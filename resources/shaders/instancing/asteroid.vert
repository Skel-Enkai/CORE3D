#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColour;
layout (location = 3) in vec2 aTex;
layout (location = 4) in mat4 instanceMatrix;
layout (location = 8) in mat4 rotationMatrix;

out DATA 
{
  vec3 Normal;
  vec3 Colour;
  vec2 TexCoord;
  mat4 projection;
  mat4 lightProjection;
} data_out;

// The final view matrix formed by the Camera 
uniform mat4 camMatrix;

void main()
{
  gl_Position = instanceMatrix * rotationMatrix * vec4(aPos, 1.0);
  data_out.Normal = vec3(rotationMatrix * vec4(aNormal, 1.0));
  data_out.Colour = aColour;
  data_out.TexCoord = aTex;
  data_out.projection = camMatrix;
}
