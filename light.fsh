#version 330 core

in vec2 outUV;

out vec4 FragColor;

uniform vec3 lightColor;
uniform sampler2D tex;

void main()
{
    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * lightColor;

    FragColor = vec4(ambient, 0.0f) + texture(tex, outUV); // set alle 4 vector values to 1.0
}