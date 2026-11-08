#version 330 core

// Input attributes
in vec3 position_vs;

// Uniforms
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position_vs, 1.0);
}