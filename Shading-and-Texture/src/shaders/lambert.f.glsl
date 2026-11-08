#version 330 core

//inputs
in vec3 normal_fs;
in vec3 world_position;

// Uniforms

uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 light_color;
uniform float light_intensity;
uniform vec3 light_position;

// Outputs

out vec4 fragment_color;

void main()
{
	vec3 n = normalize(normal_fs);
	vec3 l = normalize(light_position - world_position);
	float d = max(dot(n, l), 0.0);
	vec3 diffuse_term = diffuse_color * light_color * light_intensity * d;
	fragment_color = vec4((ambient_color + diffuse_term), 1.0);
}