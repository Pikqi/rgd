#version 330 core

// Positions/Coordinates
layout(location = 0) in vec3 aPos;
// Colors
layout(location = 1) in vec3 aColor;
// Texture Coordinates
layout(location = 2) in vec2 aTex;
// Normals (not necessarily normalized)
layout(location = 3) in vec3 aNormal;

// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the Fragment Shader
out vec2 texCoord;
// Outputs the normal for the Fragment Shader
out vec3 normal;
// Outputs the current position for the Fragment Shader
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  fragPos = vec3(model * vec4(aPos, 1.0));
  gl_Position = projection * view * vec4(fragPos, 1.0);

  texCoord = aTex;
  normal = aNormal;
  color = aColor;
}
