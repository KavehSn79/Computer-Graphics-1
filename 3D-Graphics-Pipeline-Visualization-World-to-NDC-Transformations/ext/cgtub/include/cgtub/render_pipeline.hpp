#pragma once

#include "attribute_buffer.hpp"
#include "texture_buffer.hpp"

namespace cgtub
{

class RenderPipeline
{
public:
    RenderPipeline(GLFWwindow* window, char const* vshader, char const* fshader);

    bool update_program(char const* vshader, char const* fshader);

    bool has_attribute(std::string_view name);

    template<typename T>
    bool bind_attribute(std::string_view name, AttributeBuffer<T> const& buffer);

    bool has_uniform(std::string_view name);

    template<typename T>
    bool set_uniform(std::string_view name, T&& value);

    bool has_texture(std::string_view name);

    bool bind_texture(std::string_view name, Texture const& texture);

    void render(AttributeBuffer<glm::u32vec3> const& indices, std::optional<Rect> viewport = std::nullopt);

private:
    GLFWwindow* m_window;
    GLuint      m_program;
    GLuint      m_vao;
};

} // namespace cgtub