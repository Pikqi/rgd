#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

void main()
{
  float ambientStrenght = 0.3;
  vec4 ambient = ambientStrenght * lightColor;

  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(lightPos - fragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec4 diffuse = diff * lightColor;

  float specularStrength = 0.5;
  vec3 viewDir = normalize(camPos - fragPos);
  vec3 lightReflect = reflect(-lightDir, norm);
  float spec = pow(max(dot(lightReflect, viewDir), 0.0), 32.0);
  vec4 specular = specularStrength * spec * lightColor;

  FragColor = vec4(color, 1.0f) * (ambient + diffuse + specular);
}