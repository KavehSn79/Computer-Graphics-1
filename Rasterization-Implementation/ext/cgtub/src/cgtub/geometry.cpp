#include "cgtub/geometry.hpp"

#include <algorithm>
#include <numbers>

namespace cgtub
{

void create_box_geometry(glm::vec3 scale, std::vector<glm::vec3>* positions, std::vector<glm::u32vec3>* indices, std::vector<glm::vec3>* normals, std::vector<glm::vec2>* uvs)
{
    // Generate vertex positions
    positions->clear();
    positions->reserve(8);
    for (size_t i = 0; i < 8; i++)
    {
        glm::vec3 position{i & 1, (i >> 1) & 1, (i >> 2) & 1};
        positions->emplace_back(scale * (glm::vec3(2) * position - glm::vec3(1)));
    }

    // Generate face indices
    indices->clear();
    indices->reserve(12);

    // Back
    indices->emplace_back(2, 1, 0);
    indices->emplace_back(3, 1, 2);

    // Top
    indices->emplace_back(2, 6, 7);
    indices->emplace_back(7, 3, 2);

    // Left
    indices->emplace_back(0, 4, 2);
    indices->emplace_back(2, 4, 6);

    // Right
    indices->emplace_back(3, 7, 5);
    indices->emplace_back(5, 1, 3);

    // Bottom
    indices->emplace_back(5, 4, 0);
    indices->emplace_back(5, 0, 1);

    // Front
    indices->emplace_back(4, 5, 6);
    indices->emplace_back(6, 5, 7);

    // Generate normals if requested
    if (normals)
    {
        normals->clear();
        normals->resize(8);

        std::transform(positions->begin(), positions->end(), normals->begin(),
                       [](glm::vec3 const& pos)
                       { return glm::normalize(pos); });
    }

    // Generate uv coordinates if requested
    if (uvs)
    {
        uvs->clear();
        uvs->resize(8);

        for (size_t i = 0; i < 8; i++)
        {
            uvs->emplace_back(i & 1, (i >> 1) & 1);
        }
    }
}

void create_box_geometry(float scale, std::vector<glm::vec3>* positions, std::vector<glm::u32vec3>* indices)
{
    create_box_geometry(glm::vec3(scale), positions, indices);
}

void create_sphere_geometry(unsigned int n, unsigned int m, glm::vec3 scale, std::vector<glm::vec3>* positions, std::vector<glm::u32vec3>* indices, std::vector<glm::vec3>* normals, std::vector<glm::vec2>* uvs)
{
    // Generate vertex positions
    positions->clear();
    positions->reserve(n * m);
    for (unsigned int i = 0; i < n; i++)
    {
        float theta = static_cast<float>(i) / (n - 1) * std::numbers::pi_v<float>;
        for (unsigned int j = 0; j < m; j++)
        {
            float phi = static_cast<float>(j) / m * 2 * std::numbers::pi_v<float>;

            float x = scale.x * std::sin(theta) * std::sin(phi);
            float y = scale.y * std::cos(theta);
            float z = scale.z * std::sin(theta) * std::cos(phi);
            positions->emplace_back(x, y, z);
        }
    }

    // Generate face indices
    indices->clear();
    indices->reserve(2 * (n - 1) * m);
    for (unsigned int i = 0; i < n - 1; i++)
    {
        unsigned int base = i * m;
        for (unsigned int j = 0; j < m; j++)
        {
            unsigned int vi = base + j;
            unsigned int vj = base + (j + 1) % m;
            unsigned int vk = base + m + j;
            unsigned int vl = base + m + (j + 1) % m;

            if (i > 0)
                indices->emplace_back(vk, vj, vi);
            if (i < n - 2)
                indices->emplace_back(vl, vj, vk);
        }
    }

    // Generate normals if requested
    if (normals)
    {
        normals->clear();
        normals->resize(n * m);
        std::transform(positions->begin(), positions->end(), normals->begin(),
                       [](glm::vec3 const& pos)
                       { return glm::normalize(pos); });
    }

    // Generate uv coordinates if requested
    if (uvs)
    {
        uvs->clear();
        uvs->resize(n * m);
        for (unsigned int i = 0; i < n; i++)
        {
            float theta = static_cast<float>(i) / (n - 1) * std::numbers::pi_v<float>;
            for (unsigned int j = 0; j < m; j++)
            {
                float phi = static_cast<float>(j) / m * 2 * std::numbers::pi_v<float>;

                float u = phi / std::numbers::pi_v<float> * .5f;
                float v = 1.f - theta / std::numbers::pi_v<float>;

                uvs->emplace_back(u, v);
            }
        }
    }
}

void create_sphere_geometry(float scale, std::vector<glm::vec3>* positions, std::vector<glm::u32vec3>* indices)
{
    create_sphere_geometry(16, 16, glm::vec3(scale), positions, indices);
}

void create_torus_geometry(unsigned int n, unsigned int m, glm::vec3 r, glm::vec3 R, std::vector<glm::vec3>* positions, std::vector<glm::u32vec3>* indices, std::vector<glm::vec3>* normals, std::vector<glm::vec2>* uvs)
{

    // Generate vertex positions
    positions->clear();
    positions->reserve(n * m);
    for (unsigned int i = 0; i < n; i++)
    {
        float theta = static_cast<float>(i) / n * 2.f * std::numbers::pi_v<float>;
        for (unsigned int j = 0; j < m; j++)
        {
            float phi = static_cast<float>(j) / m * 2.f * std::numbers::pi_v<float>;
            float x   = (R.x + r.x * cos(phi)) * cos(theta);
            float y   = (R.y + r.y * cos(phi)) * sin(theta);
            float z   = r.z * sin(phi);

            positions->emplace_back(x, y, z);
        }
    }

    // Generate face indices
    indices->clear();
    indices->reserve(n * m * 6);
    for (unsigned int i = 0; i < n; i++)
    {
        for (unsigned int j = 0; j < m; j++)
        {
            unsigned int vi = i * m + j;
            unsigned int vj = ((i + 1) % n) * m + j;
            unsigned int vk = (vi + 1) % (n * m);
            unsigned int vl = (vj + 1) % (n * m);

            indices->emplace_back(vi, vj, vk);
            indices->emplace_back(vj, vl, vk);
        }
    }

    // Generate normals if requested
    if (normals)
    {
        normals->clear();
        normals->reserve(n * m);
        for (unsigned int i = 0; i < n; i++)
        {
            float theta = static_cast<float>(i) / n * 2.f * std::numbers::pi_v<float>;
            for (unsigned int j = 0; j < m; j++)
            {
                float phi = static_cast<float>(j) / m * 2.f * std::numbers::pi_v<float>;
                float x   = r.x * cos(phi) * cos(theta);
                float y   = r.y * cos(phi) * sin(theta);
                float z   = r.z * sin(phi);

                normals->emplace_back(glm::normalize(glm::vec3(x, y, z)));
            }
        }
    }

    // Generate uv coordinates if requested
    if (uvs)
    {
        uvs->clear();
        uvs->reserve(n * m);
        for (unsigned int i = 0; i < n; i++)
        {
            float theta = static_cast<float>(i) / n * 2.f * std::numbers::pi_v<float>;
            for (unsigned int j = 0; j < m; j++)
            {
                float phi = static_cast<float>(j) / m * 2.f * std::numbers::pi_v<float>;

                float u = phi / std::numbers::pi_v<float> * .5f;
                float v = 1.f - theta / std::numbers::pi_v<float> * 0.5f;

                uvs->emplace_back(u, v);
            }
        }
    }
}

void create_torus_geometry(float r, float R, std::vector<glm::vec3>* positions, std::vector<glm::u32vec3>* indices)
{
    return create_torus_geometry(16, 16, glm::vec3(r), glm::vec3(R), positions, indices);
}

} // namespace cgtub