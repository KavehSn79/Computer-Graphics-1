#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace ex3
{

using GuiChanges = uint32_t;

/**
 * \brief Update the Graphical User Interface (GUI) and retrieve new values for the parameters.
 *
 * All non-const parameters are input/output, meaning their value will be used to display the GUI and
 * they will be set to the new value, as implied by user interaction with the GUI (in the previous frame).
 *
 * \return Object that tracks changes to the parameters.
 */
GuiChanges gui(int* subsampling_rate, bool* use_random_triangle_colors, bool* use_z_buffer, bool* show_z_buffer, bool* cull_negative_w, bool* cull_front_faces);

/**
 * \brief Query if an interaction with the GUI has changed a parameter value.
 *
 * Example usage:
 * \code{.cpp}
 * int   foo = ...;
 * float bar = ...;
 *
 * GuiChanges gui_changes = gui(&foo, &bar);
 *
 * if(has_gui_changed_parameter(gui_changes, 0))
 * {
 *     // Parameter `foo` was changed
 * }
 *
 * if(has_gui_changed_parameter(gui_changes, 1))
 * {
 *     // Parameter `bar` was changed
 * }
 * \endcode
 *
 * \param[in] changes         The \c GuiChanges returned by a call to \c gui(...)
 * \param[in] parameter_index The 0-based index of the parameter to query for changes
 */
bool has_gui_changed_parameter(GuiChanges gui_changes, uint32_t parameter_index);

// Generate a unique, random color for the triangle with index `triangle_index`
glm::vec3 get_random_color(size_t triangle_index);

glm::vec3 generate_dummy_color(int x, int y, int width, int height);

} // namespace ex3