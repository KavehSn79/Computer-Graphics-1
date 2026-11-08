#include "helper.hpp"

#include <complex>
#include <random>

#include <glm/gtc/constants.hpp>

#include <imgui.h>

namespace ex3
{

GuiChanges gui(int* subsampling_rate, bool* use_random_triangle_colors, bool* use_z_buffer, bool* show_z_buffer, bool* cull_behind_camera, bool* cull_front_faces)
{
    GuiChanges changes{0};

    ImGui::Begin("Exercise 3");

    if (ImGui::SliderInt("Subsampling Rate", subsampling_rate, 1, 12))
        changes |= 0b000001;

    if (ImGui::Checkbox("Use Random Triangle Colors", use_random_triangle_colors))
        changes |= 0b000010;
    if (ImGui::Checkbox("Use z-Buffer", use_z_buffer))
        changes |= 0b000100;
    if (ImGui::Checkbox("Show z-Buffer", show_z_buffer))
        changes |= 0b001000;
    if (ImGui::Checkbox("Cull Behind Camera", cull_behind_camera))
        changes |= 0b010000;
    if (ImGui::Checkbox("Cull Front Faces", cull_front_faces))
        changes |= 0b100000;

    ImGuiIO& io = ImGui::GetIO();
    // TODO: Report FPS with 2 decimal precision
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    return changes;
}

bool has_gui_changed_parameter(GuiChanges gui_changes, uint32_t parameter_index)
{
    if (parameter_index >= 32)
        return false;

    return static_cast<bool>(gui_changes & (1 << parameter_index));
}

glm::vec3 get_random_color(size_t triangle_index)
{
    static std::default_random_engine            engine(0);
    static std::uniform_real_distribution<float> distribution;
    static std::vector<glm::vec3>                random_colors;
    // Well, "unique" if the number of triangles is below 1024 :)
    constexpr int                                num_random_colors = 1024;

    // Fill the color array on the first call
    if (random_colors.empty())
    {
        random_colors.resize(num_random_colors);
        for (size_t i = 0; i < random_colors.size(); ++i)
        {
            random_colors[i] = glm::vec3(distribution(engine), distribution(engine), distribution(engine));
        }
    }

    return random_colors[triangle_index % num_random_colors];
}

glm::vec3 generate_dummy_color(int x, int y, int width, int height)
{
    float time = 1.5f;

    float const t = std::fmod(0.5f * time, glm::two_pi<float>());

    float const xf = 2 * x / static_cast<float>(width) - 1.f;
    float const yf = 2 * y / static_cast<float>(height) - 1.f;

    std::complex<float> const c(0.5f * std::sin(t), 0.65f);
    std::complex<float>       z(xf, yf);
    unsigned int const        maxIterations = 20;
    unsigned int              i             = 0;
    for (; i < maxIterations && std::abs(z) <= 10.f; i++)
    {
        z = z * z + c;
    }

    return glm::vec3(0, i / static_cast<float>(maxIterations), std::cos(2.f * t) + 1);
}

} // namespace ex3