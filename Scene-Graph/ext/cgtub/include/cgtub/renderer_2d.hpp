#pragma once

#include <optional>
#include <span>

#include <glm/glm.hpp>

#include "cgtub/fwd.hpp"
#include "cgtub/primitives.hpp"

namespace cgtub
{

class ImageRenderer
{
public:
    ImageRenderer(Canvas& canvas);

    void update_image(std::span<glm::vec3 const> pixels, unsigned int width, unsigned int height);

    void render(std::span<glm::vec3 const> pixels, unsigned int width, unsigned int height);

private:
    Canvas& m_canvas;
    GLuint  m_texture{0u};
    GLuint  m_program{0u};
    GLuint  m_vao{0u};
};

} // namespace cgtub