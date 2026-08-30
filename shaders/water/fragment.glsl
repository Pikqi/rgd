#version 330 core

in vec3 vWorldPos;

out vec4 FragColor;

uniform vec3 uSunDir;
uniform vec3 uSunColor;
uniform vec3 uCameraPos;
uniform float uTime;
uniform float uWaveAmplitude;
uniform float uShininess;
uniform vec3 uWaterColor;
uniform vec3 uHorizonColor;
uniform float uSeaLevel;
uniform float uDistortionStrength;
uniform float uDepthDarkness;
uniform float uMaxWaterDepth;

uniform sampler2D uSceneColorTex; // snapshot color (refraction source)
uniform sampler2D uSceneDepthTex; // snapshot depth (for water depth)
uniform mat4 uInvViewProj; // reconstruct world pos from depth

// Wave field: three directional sine waves. Directions are unit vectors in XZ.
const vec3 WAVE_DIR[3] = vec3[3](vec3(1.0, 0.0, 0.6),
    vec3(-0.7, 0.0, 1.0),
    vec3(0.3, 0.0, -0.9));
const float WAVE_FREQ[3] = float[3](0.15, 0.22, 0.34);
const float WAVE_SPEED[3] = float[3](0.8, 1.1, 1.4);
const float WAVE_AMP_SCALE[3] = float[3](1.0, 0.6, 0.35);

// Lighting tuning.
const float FRESNEL_POWER = 5.0; // Schlick approximation exponent
const float AMBIENT_FRACTION = 0.3; // base water color always visible
const float DIFFUSE_FRACTION = 0.7; // diffuse lighting strength
const float SPECULAR_INTENSITY = 2.0; // sun glint brightness multiplier
const float FRESNEL_TINT_STRENGTH = 0.25; // horizon color at grazing angles
const float FADE_START_DISTANCE = 200.0; // distance where horizon fade begins
const float FADE_RANGE = 250.0; // distance over which fade completes

vec3 waveNormal(vec2 worldPositionXZ, float time, float amplitudeScale)
{
  vec2 heightGradient = vec2(0.0);
  for (int waveIndex = 0; waveIndex < 3; ++waveIndex)
  {
    vec2 waveDirectionXZ = WAVE_DIR[waveIndex].xz;
    float waveAmplitude = amplitudeScale * WAVE_AMP_SCALE[waveIndex];
    float waveFrequency = WAVE_FREQ[waveIndex];
    float waveSpeed = WAVE_SPEED[waveIndex];
    float phase = dot(waveDirectionXZ, worldPositionXZ)
        * waveFrequency + time * waveSpeed;
    float cosPhase = cos(phase);
    heightGradient.x += waveAmplitude * waveDirectionXZ.x
        * waveFrequency * cosPhase;
    heightGradient.y += waveAmplitude * waveDirectionXZ.y
        * waveFrequency * cosPhase;
  }
  return normalize(vec3(-heightGradient.x, 1.0, -heightGradient.y));
}

// Reconstruct world-space position from a depth sample at screenUv.
vec3 worldPosFromDepth(vec2 screenUv, float depthSample)
{
  vec4 clipPos = uInvViewProj
      * vec4(screenUv * 2.0 - 1.0, depthSample * 2.0 - 1.0, 1.0);
  return clipPos.xyz / clipPos.w;
}

void main()
{
  vec3 surfaceNormal = waveNormal(vWorldPos.xz, uTime, uWaveAmplitude);
  vec3 viewDirection = normalize(uCameraPos - vWorldPos);
  vec3 sunDirection = normalize(uSunDir);
  vec3 halfVector = normalize(sunDirection + viewDirection);

  // Screen-space refraction sampling (with wave-based UV distortion)
  vec2 screenSize = vec2(textureSize(uSceneColorTex, 0));
  vec2 screenUv = gl_FragCoord.xy / screenSize;
  vec2 refractedUv = screenUv + surfaceNormal.xz * uDistortionStrength;
  vec3 refractedSceneColor = texture(uSceneColorTex, refractedUv).rgb;

  //  Water depth from scene depth (undistorted UV; depth is geometric)
  float sceneDepthSample = texture(uSceneDepthTex, screenUv).r;
  vec3 terrainWorldPos = worldPosFromDepth(screenUv, sceneDepthSample);
  float waterDepth = clamp(uSeaLevel - terrainWorldPos.y, 0.0, uMaxWaterDepth);
  float depthFactor = waterDepth / uMaxWaterDepth; // 0 = shore, 1 = deep

  //  Lighting (reuse existing constants)
  float diffuseTerm = max(dot(surfaceNormal, sunDirection), 0.0);
  float specularTerm = pow(max(dot(surfaceNormal, halfVector), 0.0), uShininess);
  float fresnelTerm = pow(1.0 - max(dot(surfaceNormal, viewDirection), 0.0),
      FRESNEL_POWER);

  //  Water surface color: lit, darkened by depth
  vec3 waterColor = uWaterColor
      * (AMBIENT_FRACTION + DIFFUSE_FRACTION * diffuseTerm);
  waterColor *= mix(1.0, 1.0 - uDepthDarkness, depthFactor);

  // Sun glint sits on the surface layer.
  vec3 sunGlint = uSunColor * specularTerm * SPECULAR_INTENSITY;
  vec3 horizonTint = uHorizonColor * fresnelTerm * FRESNEL_TINT_STRENGTH;
  vec3 surfaceLayer = waterColor + sunGlint + horizonTint;

  //  Mix refraction (bottom) vs surface layer
  // Head-on + shallow  -> see refracted bottom
  // Grazing or deep    -> surface layer dominates (absorption + Fresnel)
  float refractionVisibility =
    (1.0 - fresnelTerm) * (1.0 - depthFactor * uDepthDarkness);
  vec3 surfaceColor = mix(surfaceLayer, refractedSceneColor,
      refractionVisibility);

  //  Distance fade so the plane edge blends into the horizon
  float distanceToFragment = length(uCameraPos - vWorldPos);
  float horizonFade = clamp(1.0 - (distanceToFragment - FADE_START_DISTANCE)
          / FADE_RANGE, 0.0, 1.0);
  surfaceColor = mix(uHorizonColor, surfaceColor, horizonFade);

  FragColor = vec4(surfaceColor, 1.0);
}
