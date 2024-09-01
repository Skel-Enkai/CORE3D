#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightProjection;
// Model Matrices
uniform mat4 initMatrix;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
  gl_Position = lightProjection * initMatrix * translation * rotation * scale * vec4(aPos, 1.0);
}
