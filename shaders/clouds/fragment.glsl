#version 330 core

in vec2 vNDC;

out vec4 FragColor;

uniform mat4 uInvViewProj;
uniform vec3 uCameraPos;
uniform vec3 uSunDir;
uniform vec3 uSunColor;

uniform float uLayerStart;
uniform float uLayerEnd;
uniform float uCoverage;
uniform int uStepCount;
uniform float uTime;
uniform bool uOffsetStart;

float hash12(vec2 p)
{
  vec3 p3 = fract(vec3(p.xyx) * 0.1031);
  p3 += dot(p3, p3.yzx + 33.33);
  return fract((p3.x + p3.y) * p3.z);
}

float rand(vec3 p)
{
  return fract(sin(dot(p, vec3(12.345, 67.89, 412.12))) * 42123.45) * 2.0 -
    1.0;
}

float noise(vec3 p)
{
  vec3 u = floor(p);
  vec3 v = fract(p);
  vec3 s = smoothstep(0.0, 1.0, v);

  float a = rand(u);
  float b = rand(u + vec3(1.0, 0.0, 0.0));
  float c = rand(u + vec3(0.0, 1.0, 0.0));
  float d = rand(u + vec3(1.0, 1.0, 0.0));
  float e = rand(u + vec3(0.0, 0.0, 1.0));
  float f = rand(u + vec3(1.0, 0.0, 1.0));
  float g = rand(u + vec3(0.0, 1.0, 1.0));
  float h = rand(u + vec3(1.0, 1.0, 1.0));

  return mix(mix(mix(a, b, s.x), mix(c, d, s.x), s.y),
    mix(mix(e, f, s.x), mix(g, h, s.x), s.y),
    s.z);
}

float fbm(vec3 p) {
  vec3 q = p + uTime * 0.5 * vec3(1.0, -0.2, -1.0);

  float f = 0.0;
  float scale = 0.5;
  float factor = 2.02;

  for (int i = 0; i < 6; i++) {
    f += scale * noise(q);
    q *= factor;
    factor += 0.21;
    scale *= 0.5;
  }

  return f;
}

float sampleDensity(vec3 p) {
  float f = fbm(p);

  float coverage = clamp(uCoverage, 0, 1);
  float density = smoothstep(coverage, coverage + 0.15, f);

  return density;
}

vec4 raymarch(vec3 rayOrigin, vec3 rayDirection, float tEnter, float tExit) {
  float hashOffset = 0;
  if (uOffsetStart) {
    hashOffset = hash12(gl_FragCoord.xy) * 0.6;
  }
  vec3 p = rayOrigin + (tEnter + hashOffset) * rayDirection;
  float stepSize = (tExit - tEnter) / float(uStepCount);

  vec4 res = vec4(0.0);

  for (int i = 0; i < uStepCount; i++) {
    float density = sampleDensity(p);

    if (density > 0.0) {
      float diffuse = clamp((density - sampleDensity(p + 0.3 * uSunDir)) / 0.3, 0.0, 1.0);
      vec3 lin = vec3(0.60, 0.60, 0.75) * 1.1 + 0.8 * uSunColor * diffuse;
      vec4 color = vec4(mix(vec3(1.0, 1.0, 1.0), vec3(0.3, 0.3, 0.3), density), density);
      color.rgb *= lin;
      color.rgb *= color.a;
      res += color * (1.0 - res.a);
    }

    p += stepSize * rayDirection;
  }

  return res;
}

void main()
{
  // Reconstruct world-space view ray.
  vec4 nearH = uInvViewProj * vec4(vNDC, -1.0, 1.0);
  vec4 farH = uInvViewProj * vec4(vNDC, 1.0, 1.0);
  vec3 worldNear = nearH.xyz / nearH.w;
  vec3 worldFar = farH.xyz / farH.w;
  vec3 rayDir = normalize(worldFar - worldNear);
  vec3 rayOrigin = uCameraPos;

  float tEnter, tExit;
  if (abs(rayDir.y) < 0.0001)
  {
    // Ray parallel to slab, only hit if inside slab.
    if (rayOrigin.y > uLayerStart && rayOrigin.y < uLayerEnd)
    {
      tEnter = 0.0;
      tExit = 100000.0;
    }
  }
  else
  {
    float t1 = (uLayerStart - rayOrigin.y) / rayDir.y;
    float t2 = (uLayerEnd - rayOrigin.y) / rayDir.y;
    tEnter = min(t1, t2);
    tExit = max(t1, t2);

    tEnter = max(tEnter, 0.0);
    tExit = max(tExit, 0.0);
  }

  vec4 res = raymarch(rayOrigin, rayDir, tEnter, tExit);
  FragColor = res;
}
