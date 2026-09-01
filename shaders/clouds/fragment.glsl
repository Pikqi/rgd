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

uniform sampler2D uSceneDepth;

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

const float MAX_MARCH_DIST = 400.0;

bool intersectSlab(vec3 ro, vec3 rd, float yBottom, float yTop,
  out float tNear, out float tFar)
{
  float dy = abs(rd.y) < 0.0004 ? 0.0004 : rd.y;
  float t0 = (yBottom - ro.y) / dy;
  float t1 = (yTop - ro.y) / dy;

  tNear = max(min(t0, t1), 0.0);
  tFar = min(max(t0, t1), tNear + MAX_MARCH_DIST);
  return tFar > tNear;
}

vec4 raymarch(vec3 ro, vec3 rd, float tNear, float tFar)
{
  float t = tNear;
  if (uOffsetStart)
    t += hash12(gl_FragCoord.xy) * 0.6;

  float stepSize = (tFar - tNear) / float(uStepCount);
  vec4 acc = vec4(0.0);

  for (int i = 0; i < uStepCount; i++)
  {
    vec3 p = ro + t * rd;
    float density = sampleDensity(p);

    if (density > 0.0)
    {
      float diffuse = clamp((density - sampleDensity(p + 0.3 * uSunDir)) / 0.3, 0.0, 1.0);
      vec3 lin = vec3(0.60, 0.60, 0.75) * 1.1 + 0.8 * uSunColor * diffuse;
      vec3 albedo = mix(vec3(1.0), vec3(0.3), density);
      vec4 sample_ = vec4(albedo * lin, 1.0) * density;
      acc += sample_ * (1.0 - acc.a);

      if (acc.a > 0.99)
        break;
    }

    t += stepSize;
  }

  return acc;
}

void main()
{
  vec4 nearH = uInvViewProj * vec4(vNDC, -1.0, 1.0);
  vec4 farH = uInvViewProj * vec4(vNDC, 1.0, 1.0);
  vec3 rayDir = normalize(farH.xyz / farH.w - nearH.xyz / nearH.w);

  float tNear, tFar;
  if (!intersectSlab(uCameraPos, rayDir, uLayerStart, uLayerEnd, tNear, tFar))
  {
    FragColor = vec4(0.0);
    return;
  }

  vec2 screenUV = vNDC * 0.5 + 0.5;
  float sceneDepth = texture(uSceneDepth, screenUV).r;
  if (sceneDepth < 1.0)
  {
    vec4 hitH = uInvViewProj * vec4(vNDC, sceneDepth * 2.0 - 1.0, 1.0);
    float sceneDist = length(hitH.xyz / hitH.w - uCameraPos);
    if (tNear >= sceneDist)
    {
      FragColor = vec4(0.0);
      return;
    }
    tFar = min(tFar, sceneDist);
  }

  FragColor = raymarch(uCameraPos, rayDir, tNear, tFar);
}
