#version 330 core
in vec2 vUv;
out vec4 FragColor;

uniform sampler2D uTex;

uniform float uCurvature;         // screen bulge, 0 = flat, ~0.1 typical
uniform float uScanlineIntensity; // 0 = off, ~0.3 typical
uniform float uScanlineCount;     // number of dark bands, ~480
uniform float uAberration;        // RGB split in UV units, ~0.002
uniform float uVignette;          // edge darkening, 0..1
uniform float uBrightness;        // gain to compensate the darkening

const float PI = 3.14159265;

// Barrel distortion: push sampled coords outwards with radius so the picture
// looks like it bulges towards the viewer.
vec2 curve(vec2 uv, float k)
{
  uv = uv * 2.0 - 1.0;
  uv *= 1.0 + k * dot(uv, uv);
  return uv * 0.5 + 0.5;
}

void main()
{
  vec2 uv = curve(vUv, uCurvature);

  // Outside the tube is black.
  if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
  {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }

  // Chromatic aberration: split channels along the radial direction.
  vec2 dir = uv - 0.5;
  vec3 col;
  col.r = texture(uTex, uv + dir * uAberration).r;
  col.g = texture(uTex, uv).g;
  col.b = texture(uTex, uv - dir * uAberration).b;

  // Horizontal scanlines.
  float scan = 0.5 + 0.5 * sin(uv.y * uScanlineCount * PI);
  col *= 1.0 - uScanlineIntensity * scan;

  // Vignette.
  col *= clamp(1.0 - uVignette * dot(dir, dir) * 2.0, 0.0, 1.0);

  col *= uBrightness;

  FragColor = vec4(col, 1.0);
}
