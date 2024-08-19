#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 Position;
out vec3 Normal;
out vec3 Colour;
out vec2 TexCoord;

in DATA 
{
  vec3 Normal;
  vec3 Colour;
  vec2 TexCoord;
  mat4 projection;
} data_in[];

void main()
{
  Position = vec3(gl_in[0].gl_Position);
  gl_Position = data_in[0].projection * gl_in[0].gl_Position;
  Normal = data_in[0].Normal;
  Colour = data_in[0].Colour;
  TexCoord = data_in[0].TexCoord;
  EmitVertex();

  Position = vec3(gl_in[1].gl_Position);
  gl_Position = data_in[1].projection * gl_in[1].gl_Position;
  Normal = data_in[1].Normal;
  Colour = data_in[1].Colour;
  TexCoord = data_in[1].TexCoord;
  EmitVertex();

  Position = vec3(gl_in[2].gl_Position);
  gl_Position = data_in[2].projection * gl_in[2].gl_Position;
  Normal = data_in[2].Normal;
  Colour = data_in[2].Colour;
  TexCoord = data_in[2].TexCoord;
  EmitVertex();

  EndPrimitive();
}
