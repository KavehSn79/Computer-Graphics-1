#include "cgtub/render_pipeline.hpp"

#include <iostream>

namespace cgtub
{

RenderPipeline::RenderPipeline(GLFWwindow* window, char const* vshader, char const* fshader)
    : m_window(window)
    , m_program(0u)
    , m_vao(0u)
{
    create_program(vshader, fshader, &m_program);
    glGenVertexArrays(1, &m_vao);
}

bool RenderPipeline::update_program(char const* vshader, char const* fshader)
{
    return create_program(vshader, fshader, &m_program);
}

bool RenderPipeline::has_attribute(std::string_view name)
{
    return glGetAttribLocation(m_program, name.data()) >= 0;
}

bool RenderPipeline::has_uniform(std::string_view name)
{
    return glGetUniformLocation(m_program, name.data()) >= 0;
}

bool RenderPipeline::has_texture(std::string_view name)
{
    return glGetUniformLocation(m_program, name.data()) >= 0;
}

bool RenderPipeline::bind_texture(std::string_view name, Texture const& texture)
{
    GLint location = glGetUniformLocation(m_program, name.data());

    if (location < 0)
    {
        std::cerr << "RenderPipeline::bind_texture: Texture '" << name << "' is not present in the current shaders." << std::endl;
    }

    ::cgtub::bind_texture(m_program, location, texture.gl());

    return true;
}

void RenderPipeline::render(AttributeBuffer<glm::u32vec3> const& indices, std::optional<Rect> viewport)
{
    set_viewport(m_window, viewport);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(m_program);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.gl());

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(3 * indices.elements()), GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

#define IMPLEMENT_ATTRIBUTE_BINDER(type)                                                                                                                                                             \
    template<>                                                                                                                                                                                       \
    bool RenderPipeline::bind_attribute(std::string_view name, AttributeBuffer<type> const& buffer)                                                                                                   \
    {                                                                                                                                                                                                \
        GLint location = glGetAttribLocation(m_program, name.data());                                                                                                                                \
                                                                                                                                                                                                     \
        if (location < 0)                                                                                                                                                                            \
        {                                                                                                                                                                                            \
            std::cerr << "RenderPipeline::bind_attribute: Attribute '" << name << "' is not present in the current shaders. Unused attributes are potentially removed as optimization." << std::endl; \
            return false;                                                                                                                                                                            \
        }                                                                                                                                                                                            \
                                                                                                                                                                                                     \
        ::cgtub::bind_attribute<type>(m_vao, location, buffer.gl());                                                                                                                                         \
                                                                                                                                                                                                     \
        return true;                                                                                                                                                                                 \
    }

IMPLEMENT_ATTRIBUTE_BINDER(float)
IMPLEMENT_ATTRIBUTE_BINDER(double)
IMPLEMENT_ATTRIBUTE_BINDER(int)
IMPLEMENT_ATTRIBUTE_BINDER(unsigned int)

// FIXME: This does not compile. Why?
// IMPLEMENT_ATTRIBUTE_BINDER(glm::vec1)
// IMPLEMENT_ATTRIBUTE_BINDER(glm::dvec1)
// IMPLEMENT_ATTRIBUTE_BINDER(glm::i32vec1)
// IMPLEMENT_ATTRIBUTE_BINDER(glm::u32vec1)

IMPLEMENT_ATTRIBUTE_BINDER(glm::vec2)
IMPLEMENT_ATTRIBUTE_BINDER(glm::dvec2)
IMPLEMENT_ATTRIBUTE_BINDER(glm::i32vec2)
IMPLEMENT_ATTRIBUTE_BINDER(glm::u32vec2)

IMPLEMENT_ATTRIBUTE_BINDER(glm::vec3)
IMPLEMENT_ATTRIBUTE_BINDER(glm::dvec3)
IMPLEMENT_ATTRIBUTE_BINDER(glm::i32vec3)
IMPLEMENT_ATTRIBUTE_BINDER(glm::u32vec3)

IMPLEMENT_ATTRIBUTE_BINDER(glm::vec4)
IMPLEMENT_ATTRIBUTE_BINDER(glm::dvec4)
IMPLEMENT_ATTRIBUTE_BINDER(glm::i32vec4)
IMPLEMENT_ATTRIBUTE_BINDER(glm::u32vec4)

IMPLEMENT_ATTRIBUTE_BINDER(glm::mat2)
IMPLEMENT_ATTRIBUTE_BINDER(glm::mat3)
IMPLEMENT_ATTRIBUTE_BINDER(glm::mat4)

IMPLEMENT_ATTRIBUTE_BINDER(glm::dmat2)
IMPLEMENT_ATTRIBUTE_BINDER(glm::dmat3)
IMPLEMENT_ATTRIBUTE_BINDER(glm::dmat4)

template<typename T>
bool setUniformHelper(GLuint program, std::string_view name, T&& value)
{
    GLint location = glGetUniformLocation(program, name.data());

    if (location < 0)
    {
        std::cerr << "RenderPipeline::set_uniform: Uniform '" << name << "' is not present in the current shaders." << std::endl;
        return false;
    }

    ::cgtub::set_uniform(program, location, std::forward<T>(value));

    return true;
}

#define IMPLEMENT_UNIFORM_SETTER(type)                                        \
    template<>                                                                \
    bool RenderPipeline::set_uniform(std::string_view name, type&& value)      \
    {                                                                         \
        return setUniformHelper(m_program, name, std::forward<type>(value));  \
    }                                                                         \
    template<>                                                                \
    bool RenderPipeline::set_uniform(std::string_view name, type& value)       \
    {                                                                         \
        return setUniformHelper(m_program, name, value);                      \
    }                                                                         \
    template<>                                                                \
    bool RenderPipeline::set_uniform(std::string_view name, type const& value) \
    {                                                                         \
        return setUniformHelper(m_program, name, value);                      \
    }

IMPLEMENT_UNIFORM_SETTER(float)
IMPLEMENT_UNIFORM_SETTER(double)
IMPLEMENT_UNIFORM_SETTER(int)
IMPLEMENT_UNIFORM_SETTER(unsigned int)

// NOTE: ::set_uniform doesn't support these
// IMPLEMENT_UNIFORM_SETTER(glm::vec1)
// IMPLEMENT_UNIFORM_SETTER(glm::dvec1)
// IMPLEMENT_UNIFORM_SETTER(glm::i32vec1)
// IMPLEMENT_UNIFORM_SETTER(glm::u32vec1)

IMPLEMENT_UNIFORM_SETTER(glm::vec2)
IMPLEMENT_UNIFORM_SETTER(glm::dvec2)
IMPLEMENT_UNIFORM_SETTER(glm::i32vec2)
IMPLEMENT_UNIFORM_SETTER(glm::u32vec2)

IMPLEMENT_UNIFORM_SETTER(glm::vec3)
IMPLEMENT_UNIFORM_SETTER(glm::dvec3)
IMPLEMENT_UNIFORM_SETTER(glm::i32vec3)
IMPLEMENT_UNIFORM_SETTER(glm::u32vec3)

IMPLEMENT_UNIFORM_SETTER(glm::vec4)
IMPLEMENT_UNIFORM_SETTER(glm::dvec4)
IMPLEMENT_UNIFORM_SETTER(glm::i32vec4)
IMPLEMENT_UNIFORM_SETTER(glm::u32vec4)

IMPLEMENT_UNIFORM_SETTER(glm::mat2)
IMPLEMENT_UNIFORM_SETTER(glm::mat3)
IMPLEMENT_UNIFORM_SETTER(glm::mat4)

IMPLEMENT_UNIFORM_SETTER(glm::dmat2)
IMPLEMENT_UNIFORM_SETTER(glm::dmat3)
IMPLEMENT_UNIFORM_SETTER(glm::dmat4)

} // namespace cgtub