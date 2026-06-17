#version 330 core
in vec2 vUv;
out vec4 FragColor;

uniform sampler2D uTex;
uniform float uIntensity;
uniform float uRadius;
uniform float uSoftness;

void main()
{
    vec3  c        = texture(uTex, vUv).rgb;
    vec2  centered = vUv - 0.5;
    float dist     = length(centered) * 1.414;
    float vignette = smoothstep(uRadius, uRadius - uSoftness, dist);
    c *= mix(1.0, vignette, uIntensity);
    FragColor = vec4(c, 1.0);
}
