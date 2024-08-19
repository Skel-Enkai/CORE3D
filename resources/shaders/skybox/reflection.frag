#version 330 core

out vec4 FragColour;
in vec3 Normal;
in vec3 Position;

uniform vec3 camPos;
uniform samplerCube skybox;

void main()
{
  vec3 I = normalize(Position - camPos);
  vec3 R = reflect(I, normalize(Normal));
  // Left handed coordinate system
  FragColour = vec4(texture(skybox, vec3(R.x, R.y, -R.z)).rgb, 1.0);
}
