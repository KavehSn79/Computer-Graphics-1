#pragma once

#include <span>

#include "glad/glad.h"

namespace cgtub
{

template<typename T>
class AttributeBuffer
{
public:
    AttributeBuffer();

    void upload(std::span<T const> values);

    size_t elements() const;

    GLuint gl() const;

private:
    GLuint m_buffer;
    size_t m_elements;
};

} // namespace cgtub