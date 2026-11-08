#version 330 core

// Uniforms
uniform vec3 ambient_color;

// Output
out vec4 fragment_color;

void main()
{
    fragment_color = vec4(ambient_color, 1.0);
}