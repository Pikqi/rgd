#version 330 core
in vec2 vUv;
out vec4 FragColor;

uniform sampler2D uTex;

void main()
{
    FragColor = texture(uTex, vUv);
}
