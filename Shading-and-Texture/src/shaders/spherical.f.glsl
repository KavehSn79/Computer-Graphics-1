#version 330 core

// Input
in vec3 world_position;

// Uniforms
uniform sampler2D img_texture;

// Outputs
out vec4 fragment_color;

const float PI = 3.14159265358979323846;

void main() 
{
	float x = world_position.x;
    float y = world_position.y;
    float z = world_position.z;

    float theta = atan(world_position.x, world_position.z) + PI;
    float phi = atan(length(world_position.xz), world_position.y);
    
    float u = theta / (2.0 * PI);
    float v = phi / PI;
    
    vec2 uv = vec2(u, v);
    fragment_color = texture(img_texture, uv);
}
