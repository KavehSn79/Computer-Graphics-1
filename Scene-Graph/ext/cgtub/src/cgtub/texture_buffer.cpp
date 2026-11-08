#include "cgtub/texture_buffer.hpp"

namespace cgtub
{

Texture::Texture()
    : m_texture(0u)
{
    glGenTextures(1, &m_texture);
}

void Texture::upload(std::span<glm::vec3 const> values, unsigned int width, unsigned int height)
{
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, OpenGLType<glm::vec3::value_type>::value, values.data());
}

GLuint Texture::gl() const
{
    return m_texture;
}

} // namespace cgtub