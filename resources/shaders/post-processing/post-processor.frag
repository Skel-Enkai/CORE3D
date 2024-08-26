#version 330 core 

out vec4 FragColour;
in vec2 texCoord;

uniform sampler2DMS multiSampler0;
uniform int texSamples;
uniform float gamma;

ivec2 texSize = textureSize(multiSampler0);

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
  FragColour = textureMultisample(multiSampler0, uv);
  FragColour.rgb = pow(FragColour.rgb, vec3(1.0/gamma));
}       
