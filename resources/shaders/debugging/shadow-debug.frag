#version 330 core 

out vec4 FragColour;
in vec2 texCoord;

uniform sampler2D screenTexture;

float near = 0.1f;
float far = 75.0f;
// linearize the F-Depth formula: F_depth= (1/z- 1/near)/(1/far - 1/near);
float linearizeDepth(float depth)
{
  return (2.0 * near * far) / (far + near -(depth * 2.0 - 1.0) * (far - near));
}
// steepness: controls how quickly the depth value varies
// offset: controls which set value of distance depth is halfway done, (which distance represents the depth value 0.5f)
float logisticDepth(float depth)
{
  float steepness = 0.5f;
  float offset = 0.5f;
  float zVal = linearizeDepth(depth);
  return (1 / (1 + exp(-steepness * (zVal - offset))));
}
void main()
{
  float depth = texture(screenTexture, texCoord).r;
  FragColour = vec4(depth, depth, depth, 1.0);
}       
