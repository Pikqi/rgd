#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in float color_strenght;

out vec2 TexCoord;
out float col_str;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
  gl_Position = projection * view * model * vec4(vertex.x, vertex.y, vertex.z, 1.0);
  TexCoord = (vertex.xy + 0.5);
  col_str = color_strenght;
}
