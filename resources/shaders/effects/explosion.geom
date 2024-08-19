#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 Position;
out vec3 Normal;
out vec3 Colour;
out vec2 TexCoord;

uniform float time;

in DATA 
{
  vec3 Normal;
  vec3 Colour;
  vec2 TexCoord;
  mat4 projection;
} data_in[];

vec3 getNormal()
{
  vec3 vector0 = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
  vec3 vector1 = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
  return normalize(cross(vector0, vector1));
}

vec4 explode(vec4 position, vec3 normal)
{
  float magnitude = 2.0;
  vec3 direction = normal * sin(time) * magnitude;
  return position + vec4(direction, 0.0);
}
// "Explosion" effect main function
void main()
{
  vec3 surfaceNormal = getNormal();

  Position = vec3(gl_in[0].gl_Position);
  gl_Position = data_in[0].projection * explode(gl_in[0].gl_Position, surfaceNormal);
  Normal = data_in[0].Normal;
  Colour = data_in[0].Colour;
  TexCoord = data_in[0].TexCoord;
  EmitVertex();

  Position = vec3(gl_in[1].gl_Position);
  gl_Position = data_in[1].projection * explode(gl_in[1].gl_Position, surfaceNormal);
  Normal = data_in[1].Normal;
  Colour = data_in[1].Colour;
  TexCoord = data_in[1].TexCoord;
  EmitVertex();

  Position = vec3(gl_in[2].gl_Position);
  gl_Position = data_in[2].projection * explode(gl_in[2].gl_Position, surfaceNormal);
  Normal = data_in[2].Normal;
  Colour = data_in[2].Colour;
  TexCoord = data_in[2].TexCoord;
  EmitVertex();

  EndPrimitive();
}
