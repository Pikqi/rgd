#version 330 core
layout(location = 0) in vec2 vertex;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;
uniform float tex_x_scale;
uniform float tex_y_scale;
uniform float tex_x_offset;
uniform float tex_y_offset;

void main()
{
  TexCoords = vec2(vertex.x * tex_x_scale, vertex.y * tex_y_scale);
  TexCoords += vec2(tex_x_offset, tex_y_offset);
  gl_Position = projection * model * vec4(vertex, 0.0, 1.0);
}
