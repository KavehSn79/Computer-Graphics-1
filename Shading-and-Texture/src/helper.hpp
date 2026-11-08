#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <cgtub/image.hpp>

#include <glm/glm.hpp>

namespace ex4
{

enum class ShaderType : int
{
    Ambient = 0,
    Normal,
    Lambert,
    Phong,
    UV,
    Spherical,
    Environment
};

enum class TextureType : int
{
    Checker = 0,
    Colors,
    Earth,
    Indoor,
    Lava
};

enum class GeometryType : int
{
    Cube = 0,
    Sphere,
    Torus
};

/**
 * \brief Load shader code from a specified file.
 * 
 * \param[in] relative_path The path to the *.glsl shader file, relative to the 'src' directory.
 * 
 * \return A string containing the file content.
 */
std::string load_shader_from_file(std::string const& relative_path);

/**
 * \brief Load an image from a specified file.
 * 
 * \param[in]  texture_type  Texture (type) 
 * \param[out] data          An array which is populated with the texture color data
 * \param[out] width         Integer that is set to the width of the image
 * \param[out] height        Integer that is set to the height of the image
 * 
 * \return True if the image was successfully loaded, false otherwise.
 */
bool load_texture_from_file(TextureType texture_type, std::vector<glm::vec3>* data, int* width, int* height);

using GuiChanges = int;

/**
 * \brief Update the Graphical User Interface (GUI) and retrieve new values for the parameters.
 *
 * All non-const parameters are input/output, meaning their value will be used to display the GUI and
 * they will be set to the new value, as implied by user interaction with the GUI (in the previous frame).
 *
 * \return Object that tracks changes to the parameters.
 */
GuiChanges gui(ShaderType* shader_type, TextureType* texture_type, GeometryType* geometry_type,
               glm::vec3* scale,
               glm::vec3* ambient_color,
               glm::vec3* diffuse_color,
               glm::vec3* specular_color, float* specular_magnitude,
               glm::vec3* light_color, float* light_intensity, glm::vec3* light_position);

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

} // namespace ex4