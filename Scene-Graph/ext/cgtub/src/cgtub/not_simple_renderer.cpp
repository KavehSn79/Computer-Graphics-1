#include "cgtub/canvas.hpp"
#include "cgtub/event_dispatcher.hpp"
#include "cgtub/not_simple_renderer.hpp"

namespace cgtub
{

NotSimpleRenderer::NotSimpleRenderer(Canvas& canvas)
    : m_canvas(canvas)
    , m_mesh_renderer(canvas.window())
    , m_line_renderer(canvas.window())
{
}

void NotSimpleRenderer::update(float dt, EventDispatcher* dispatcher)
{
}

void NotSimpleRenderer::render_mesh(std::span<glm::vec3 const> positions, std::span<glm::u32vec3 const> indices, glm::vec3 const& color)
{
    // TODO: Maybe calculate normals?
    MeshRenderData meshData{
        .positions = positions,
        .indices   = indices,
    };
    m_mesh_renderer.render(meshData, glm::mat4(1), glm::mat4(1), MeshRenderParams{.mode = MeshRenderMode::ColorLit, .color = color}, m_canvas.viewport());
}

void NotSimpleRenderer::render_lines(std::span<glm::vec3 const> points, std::span<glm::vec3 const> colors)
{
    m_line_renderer.render(points, colors, glm::mat4(1), glm::mat4(1), m_canvas.viewport());
}

} // namespace cgtub