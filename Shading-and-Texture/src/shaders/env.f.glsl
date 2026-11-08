#version 330 core

// Inputs
in vec3 world_position;
in vec3 normal_fs;

// Uniforms
uniform sampler2D img_texture;
uniform vec3 camera_position;

// Output
out vec4 fragment_color;

const float PI = 3.14159265358979323846;

void main()
{
    vec3 view_dir = normalize(world_position - camera_position);
    vec3 n = normalize(normal_fs);
    vec3 reflected = reflect(view_dir, n);
    
    float theta = atan(reflected.x, reflected.z) + PI;  // θ ∈ [0, 2π]
    float phi = atan(length(reflected.xz), reflected.y); // φ ∈ [0, π]

    float u = theta / (2.0 * PI);
    float v = phi / PI;
    
    vec2 uv = vec2(u, v);
    fragment_color = texture(img_texture, uv);
}