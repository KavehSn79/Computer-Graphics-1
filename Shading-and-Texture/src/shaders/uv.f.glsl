#version 330 core

// Inputs
in vec2 uvs_fs;

// Uniforms
uniform sampler2D img_texture;

// Output
out vec4 fragment_color;

void main()
{
	fragment_color = texture(img_texture, uvs_fs);
}