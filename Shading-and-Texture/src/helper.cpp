#include "helper.hpp"

#include <unordered_map>

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <stb_image.h>

namespace ex4
{

std::string load_shader_from_file(const std::string& relative_path)
{
    std::string buffer;

    std::filesystem::path absolute_path = std::filesystem::path(SOURCE_DIRECTORY) / relative_path;

    std::ifstream file(absolute_path, std::ios::binary);
    if (file.is_open())
    {
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();

        buffer.resize(size + 1);
        file.seekg(0);
        file.read(buffer.data(), size);
        buffer[size] = 0;
    }

    return buffer;
}

bool load_texture_from_file(TextureType texture_type, std::vector<glm::vec3>* data, int* width, int* height)
{
    // Map from TextureType to the texture path
    static std::unordered_map<ex4::TextureType, std::string> const map_texture_to_path{
        {ex4::TextureType::Checker, "checker.jpg"},
        {ex4::TextureType::Colors, "colors.jpg"},
        {ex4::TextureType::Earth, "earth.jpg"},
        {ex4::TextureType::Indoor, "indoor.jpg"},
        {ex4::TextureType::Lava, "lava.jpg"},
    };

    std::filesystem::path absolute_path = std::filesystem::path(ASSETS_DIRECTORY) / map_texture_to_path.at(texture_type);

    int n_channels;
    float* data_ptr = stbi_loadf(absolute_path.string().c_str(), width, height, &n_channels, 3);

    if (!data_ptr)
        return false;

    data->resize(*width * *height);
    std::memcpy(data->data(), data_ptr, sizeof(glm::vec3) * data->size());

    stbi_image_free(data_ptr);

    return true;
}

GuiChanges gui(ShaderType* shader_type, TextureType* texture_type, GeometryType* geometry_type,
               glm::vec3* scale,
               glm::vec3* ambient_color,
               glm::vec3* diffuse_color,
               glm::vec3* specular_color, float* specular_magnitude,
               glm::vec3* light_color, float* light_intensity, glm::vec3* light_position)
{
    GuiChanges changes{0};

    ImGui::Begin("Exercise 4");

    if (ImGui::Combo("Shader", reinterpret_cast<int*>(shader_type), "Ambient\0Normal\0Lambert\0Phong\0UV\0Spherical\0Environment\0"))
        changes |= 1 << 0;

    if (ImGui::Combo("Texture", reinterpret_cast<int*>(texture_type), "Checker\0Colors\0Earth\0Indoor\0Lava\0"))
        changes |= 1 << 1;

    if (ImGui::Combo("Geometry", reinterpret_cast<int*>(geometry_type), "Cube\0Sphere\0Torus\0"))
        changes |= 1 << 2;

    if (ImGui::SliderFloat3("Scale (xyz)", glm::value_ptr(*scale), 0.1f, 2.f))
        changes |= 1 << 3;

    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();
        if (ImGui::ColorEdit3("Ambient Color", glm::value_ptr(*ambient_color)))
            changes |= 1 << 4;

        if (ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(*diffuse_color)))
            changes |= 1 << 5;

        if (ImGui::ColorEdit3("Specular Color", glm::value_ptr(*specular_color)))
            changes |= 1 << 6;

        if (ImGui::SliderFloat("Specular Magnitude", specular_magnitude, 1.f, 256.f, "%.0f"))
            changes |= 1 << 7;
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();
        if (ImGui::ColorEdit3("Color##Light", glm::value_ptr(*light_color)))
            changes |= 1 << 8;
        if (ImGui::SliderFloat("Intensity##Light", light_intensity, 0.f, 1.f))
            changes |= 1 << 9;
        if (ImGui::SliderFloat3("Position##Light", glm::value_ptr(*light_position), -1.f, 1.f))
            changes |= 1 << 10;
        ImGui::Unindent();
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    return changes;
}

bool has_gui_changed_parameter(GuiChanges gui_changes, unsigned int parameter_index)
{
    // TODO: parameter_index must be < 32.

    return static_cast<bool>(gui_changes & (1 << parameter_index));
}

} // namespace ex4