#version 330 core

// Inputs
in vec3 normal_fs;
in vec3 world_position;
in vec3 view_fs;

// Uniforms
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 light_color;
uniform float light_intensity;
uniform vec3 light_position;
uniform vec3 specular_color;
uniform float specular_magnitude;


// Outputs
out vec4 fragment_color;

void main()
{
    vec3 n = normalize(normal_fs);
    vec3 l = normalize(light_position - world_position);
    vec3 v = normalize(view_fs);
    
    float d = max(dot(n, l), 0.0);
    vec3 diffuse_term = diffuse_color * light_color * light_intensity * d;

    vec3 r = reflect(-l, n);  // Reflect the light direction
    float s = pow(max(dot(r, v), 0.0), specular_magnitude);
    vec3 specular_term = specular_color * light_color * light_intensity * s;
    
    fragment_color = vec4(ambient_color + diffuse_term + specular_term, 1.0);
}
