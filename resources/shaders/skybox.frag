#version 330 core 

out vec4 FragColour;
in vec3 texCoord;

uniform samplerCube cube;

void main()
{
  FragColour = texture(cube, texCoord);
}       
