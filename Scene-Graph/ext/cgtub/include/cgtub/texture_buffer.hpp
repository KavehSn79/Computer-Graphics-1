#pragma once

#include <glm/glm.hpp>
#include <span>

#include "gl_wrap.hpp"

namespace cgtub
{

class Texture
{
public:
    Texture();

    void upload(std::span<glm::vec3 const> values, unsigned int width, unsigned int height);

    GLuint gl() const;

private:
    GLuint m_texture;
};

} // namespace cgtub