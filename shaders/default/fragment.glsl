#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;
// Imports the normal from the Vertex Shader
in vec3 normal;
// Imports the current position from the Vertex Shader
in vec3 fragPos;

// Gets the Texture Unit from the main function
uniform sampler2D tex0;
// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;

void main()
{
  int shine = 32;

  float ambientStrenght = 0.3;
  vec4 ambient = ambientStrenght * lightColor;

  vec3 norm = normalize(normal);

  vec3 lightDir = normalize(lightPos - fragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec4 diffuse = diff * lightColor;

  float specularStrength = 0.5;
  vec3 viewDir = normalize(camPos - fragPos);
  vec3 lightReflect = reflect(-lightDir, norm);

  float spec = pow(max(dot(lightReflect, viewDir), 0.0), shine);

  vec4 specular = specularStrength * spec * lightColor;
  FragColor = texture(tex0, texCoord) * (ambient + diffuse + specular);
}
