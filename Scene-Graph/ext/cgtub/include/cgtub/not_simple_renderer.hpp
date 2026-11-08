#pragma once

#include <span>

#include "cgtub/fwd.hpp"
#include "cgtub/line_renderer.hpp"
#include "cgtub/mesh_renderer.hpp"

namespace cgtub
{

class NotSimpleRenderer
{
public:
    NotSimpleRenderer(Canvas& canvas);

    void update(float dt, EventDispatcher* dispatcher);

    void render_mesh(std::span<glm::vec3 const> positions, std::span<glm::u32vec3 const> indices, glm::vec3 const& color);

    void render_lines(std::span<glm::vec3 const> points, std::span<glm::vec3 const> colors);

private:
    Canvas&      m_canvas;
    MeshRenderer m_mesh_renderer;
    LineRenderer m_line_renderer;
};

} // namespace cgtub