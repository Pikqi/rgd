#version 330 core
layout(location = 0) in vec3 vertex;

out vec2 TexCoord;
void main()
{
  gl_Position = vec4(vertex.x, vertex.y, vertex.z, 1.0);
  TexCoord = (vertex.xy + 0.5);
}
