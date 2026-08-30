#version 330 core
in vec2 vUv;
out vec4 FragColor;

uniform sampler2D uTex;
uniform float uExposure;

void main()
{
  vec3 c = texture(uTex, vUv).rgb;
  c *= uExposure;
  FragColor = vec4(c, 1.0);
}
