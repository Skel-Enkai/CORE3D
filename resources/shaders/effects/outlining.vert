#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 camMatrix;
uniform mat4 initMatrix;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform float outlining;

void main()
{
  // Needs a Solidify Function for smoother edges, as normals close to corners create gaps with this method.
  // Either make a new model in blender with the Solidify function (very inefficient with memory and processing power)
  // Or make sure all normals have less than 15 degree difference with their neighbours using some interpolation 
  // function. 
  vec3 currentPos = vec3(initMatrix * translation * rotation * scale * vec4(aPos + aNormal * outlining, 1.0f));
  gl_Position = camMatrix * vec4(currentPos, 1.0);
}
