#version 330 core 

out vec4 FragColour;
in vec2 texCoord;

uniform sampler2D screenTexture;
uniform float width, height;

float offset_x = 1.0f / width;
float offset_y = 1.0f / height;

vec2 offsets[9] = vec2[]
(
  vec2(-offset_x, offset_y),  vec2(0.0f, offset_y),  vec2(offset_x, offset_y),
  vec2(-offset_x, 0.0f),      vec2(0.0f, 0.0f),      vec2(offset_x, 0.0f),
  vec2(-offset_x, -offset_y), vec2(0.0f, -offset_y), vec2(offset_x, -offset_y)
);

float kernelEdge[9] = float[]
(
  1,  1,  1,
  1, -8,  1,
  1,  1,  1
);

float kernelBlur[9] = float[]
(
  2.0/24.0, 3.0/24.0, 2.0/24.0,
  3.0/24.0, 4.0/24.0, 3.0/24.0,
  2.0/24.0, 3.0/24.0, 2.0/24.0
);

void main()
{
  vec3 colour = vec3(0.0);
  for (int i = 0; i < 9; i++)
    colour += vec3(texture(screenTexture, TexCoord.st + offsets[i])) * kernelEdge[i]; 
  FragColour = vec4(colour, abs(colour));
}       
