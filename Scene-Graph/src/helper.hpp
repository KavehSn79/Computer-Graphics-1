#pragma once

#include <glm/fwd.hpp>

namespace ex1
{

using GuiChanges = int;

/**
 * \brief Update the Graphical User Interface (GUI) and retrieve new values for the parameters.
 *
 * All parameters are input/output, meaning their value will be used to display the GUI and
 * they will be set to the new value, as implied by user interaction with the GUI (in the previous frame).
 *
 * If a parameter pointer is null (has value `nullptr`) the corresponding GUI element will not be shown.
 * 
 * \return Object that tracks changes to the parameters.
 */
GuiChanges gui(glm::vec3* scale, glm::vec3* rotation, glm::vec3* translation, bool* show_coordinate_system, bool* reset);

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
bool has_gui_changed_parameter(GuiChanges changes, unsigned int parameter_index);

} // namespace ex1
