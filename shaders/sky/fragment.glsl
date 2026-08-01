#version 330 core

in vec2 vNDC;

out vec4 FragColor;

uniform mat4 uInvViewProj;
uniform vec3 uZenithColor;
uniform vec3 uHorizonColor;
uniform vec3 uSunDir;
uniform vec3 uSunColor;

void main()
{
  vec4 nearH = uInvViewProj * vec4(vNDC, -1.0, 1.0);
  vec4 farH = uInvViewProj * vec4(vNDC, 1.0, 1.0);
  vec3 worldNear = nearH.xyz / nearH.w;
  vec3 worldFar = farH.xyz / farH.w;
  vec3 rayDir = normalize(worldFar - worldNear);

  float t = clamp(rayDir.y * 0.5 + 0.5, 0.0, 1.0);
  vec3 sky = mix(uHorizonColor, uZenithColor, t);

  vec3 sunDirN = normalize(uSunDir);
  float sunAngle = max(dot(rayDir, sunDirN), 0.0);
  float disk = smoothstep(0.998, 0.999, sunAngle);
  float halo = pow(sunAngle, 32.0) * 0.4;
  sky += uSunColor * (disk * 5.0 + halo);

  FragColor = vec4(sky, 1.0);
}
