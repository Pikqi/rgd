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
  float ambientStrength = 0.3;
  vec3 ambient = ambientStrength * lightColor.rgb;

  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(lightPos - fragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor.rgb;

  float specularStrength = 0.5;
  vec3 viewDir = normalize(camPos - fragPos);
  vec3 lightReflect = reflect(-lightDir, norm);
  float spec = pow(max(dot(lightReflect, viewDir), 0.0), 32.0);
  vec3 specular = specularStrength * spec * lightColor.rgb;

  vec3 result = color * (ambient + diffuse + specular);
  FragColor = vec4(result, 1.0);
}
