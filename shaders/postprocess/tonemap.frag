#version 330 core
in vec2 vUv;
out vec4 FragColor;

uniform sampler2D uTex;
uniform float     uExposure;
uniform int       uApplyGamma;

vec3 aces(vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 c = texture(uTex, vUv).rgb;
    c *= uExposure;
    c  = aces(c);
    if (uApplyGamma == 1)
        c = pow(c, vec3(1.0 / 2.2));
    FragColor = vec4(c, 1.0);
}
