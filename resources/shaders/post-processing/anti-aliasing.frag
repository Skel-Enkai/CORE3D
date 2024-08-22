#version 330 core 

out vec4 FragColour;
in vec2 texCoord;

uniform sampler2DMS texture0;
uniform int texSamples;

ivec2 texSize = textureSize(texture0);

vec4 textureMultisample(sampler2DMS sampler, ivec2 coord)
{
  vec4 colour = vec4(0.0);

  for (int i = 0; i < texSamples; i++)
    colour += texelFetch(sampler, coord, i);

  colour /= float(texSamples);

  return colour;
}

void main()
{
  ivec2 uv = ivec2(texCoord * texSize);
  FragColour = textureMultisample(texture0, uv); 
}       
