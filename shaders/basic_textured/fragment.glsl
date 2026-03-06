#version 330 core
in vec2 TexCoord;
in float col_str;
out vec4 color;
uniform sampler2D texture1;

void main()
{
  // color = texture(texture1, TexCoord);
  color = vec4(vec3(1.0) * col_str, 1.0f);
}
