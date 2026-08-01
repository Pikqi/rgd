#version 330 core

in vec2 vNDC;

out vec4 FragColor;

uniform sampler2D uCloudsTex;

void main()
{
    vec2 uv = vNDC * 0.5 + 0.5;
    FragColor = texture(uCloudsTex, uv);
}