#version 330 core

// Inputs
in vec3 position_vs;
in vec3 normal_vs;

// Uniforms
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat3 normal_matrix;

// Outputs
out vec3 world_position;
out vec3 normal_fs;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position_vs, 1.0);
    world_position = vec3(model_matrix * vec4(position_vs, 1.0));
    normal_fs = normal_matrix * normal_vs;
}