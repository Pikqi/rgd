#version 330 core
in vec2 vUv;
out vec4 FragColor;

uniform sampler2D uTex;

uniform float uCurvature;
uniform float uScanlineIntensity;
uniform float uScanlineCount;
uniform float uAberration;
uniform float uVignette;
uniform float uBrightness;

const float PI = 3.14159265;

vec2 curve(vec2 uv, float k)
{
  uv = uv * 2.0 - 1.0;
  uv *= 1.0 + k * dot(uv, uv);
  return uv * 0.5 + 0.5;
}

void main()
{
  vec2 uv = curve(vUv, uCurvature);

  if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
  {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }

  // Chromatic aberration
  vec2 dir = uv - 0.5;
  vec3 col;
  col.r = texture(uTex, uv + dir * uAberration).r;
  col.g = texture(uTex, uv).g;
  col.b = texture(uTex, uv - dir * uAberration).b;

  float scan = 0.5 + 0.5 * sin(uv.y * uScanlineCount * PI);
  col *= 1.0 - uScanlineIntensity * scan;

  // Vignette.
  col *= clamp(1.0 - uVignette * dot(dir, dir) * 2.0, 0.0, 1.0);

  col *= uBrightness;

  FragColor = vec4(col, 1.0);
}
