#include "helper.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include<iostream>

namespace ex1
{

GuiChanges gui(glm::vec3* scale, glm::vec3* rotation, glm::vec3* translation, bool* show_coordinate_system, bool* reset)
{

    /*std::cout << "Here?\n";*/
    GuiChanges changes{0};

    ImGui::Begin("Exercise 1");

    if (scale && ImGui::DragFloat3("Scale (X,Y,Z)", glm::value_ptr(*scale), 0.05f, 0.f, 8.f))
    {
        changes |= 0b00001;
    }

    if (rotation && ImGui::DragFloat3("Rotation (X,Y,Z)", glm::value_ptr(*rotation), 0.05f, -glm::pi<float>(), glm::pi<float>()))
    {
        changes |= 0b00010;
    }

    if (translation && ImGui::DragFloat3("Translation (X,Y,Z)", glm::value_ptr(*translation), 0.05f, -3.f, 3.f))
    {
        changes |= 0b00100;
    }

    // Checkboxes switch a bool between true and false
    if (show_coordinate_system && ImGui::Checkbox("Show Coordinate System", show_coordinate_system))
    {
        changes |= 0b01000;
    }

    if (reset)
    {
        *reset = ImGui::Button("Reset");
        if (*reset) {
            changes |= 0b10000;
        }
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

} // namespace ex1