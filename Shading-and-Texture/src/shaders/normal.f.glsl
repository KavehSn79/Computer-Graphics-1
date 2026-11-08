#version 330 core

// Input
in vec3 normal_fs;

// Output
out vec4 fragment_color;

void main()
{
    vec3 n = normalize(normal_fs);
    
    vec3 color = n * 0.5 + 0.5;
    
    fragment_color = vec4(color, 1.0);
}