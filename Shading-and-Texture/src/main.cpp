#include <filesystem>
#include <iostream>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cgtub/attribute_buffer.hpp>
#include <cgtub/camera_controller_turntable.hpp>
#include <cgtub/camera_perspective.hpp>
#include <cgtub/canvas.hpp>
#include <cgtub/event_dispatcher.hpp>
#include <cgtub/geometry.hpp>
#include <cgtub/gl_wrap.hpp>
#include <cgtub/primitives.hpp>
#include <cgtub/render_pipeline.hpp>
#include <cgtub/texture_buffer.hpp>

#include "helper.hpp"

int main(int argc, char** argv)
{
    // Create a GLFW window and an OpenGL context
    // An event dispatcher records incoming events for a window
    // (e.g. change of size or mouse cursor movement)
    GLFWwindow*             window     = nullptr;
    cgtub::EventDispatcher* dispatcher = nullptr;
    if (!cgtub::init(1200, 800, "CG1", &window, &dispatcher))
    {
        std::cerr << "Failed to initialize OpenGL window" << std::endl;
        return EXIT_FAILURE;
    }

    // A canvas is a (logical) subregion of a window.
    cgtub::Canvas canvas(window, cgtub::Extent{.x = 0.f, .y = 0.f, .width = 1.f, .height = 1.f});

    cgtub::PerspectiveCamera         camera(45, 1.f, 0.01f, 5.f);
    cgtub::TurntableCameraController controller(canvas, camera);

    // Create an OpenGL render pipeline for the window.
    cgtub::GLRenderPipeline pipeline(window);

    // Load the ambient shader code and bind it to the pipeline.
    // Internally, the pipeline compiles the shaders and links them to a program.
    std::string vertex_shader_code   = ex4::load_shader_from_file("shaders/ambient.v.glsl");
    std::string fragment_shader_code = ex4::load_shader_from_file("shaders/ambient.f.glsl");
    pipeline.set_shaders(vertex_shader_code, fragment_shader_code);

    // Create a sphere
    std::vector<glm::vec3>    mesh_positions;
    std::vector<glm::u32vec3> mesh_indices;
    std::vector<glm::vec3>    mesh_normals;
    std::vector<glm::vec2>    mesh_uvs;
    cgtub::create_box_geometry(glm::vec3(.3f), &mesh_positions, &mesh_indices, &mesh_normals, &mesh_uvs);

    // Create OpenGL attribute buffer.
    // The attribute buffer correspond to the GPU memory.
    cgtub::GLAttributeBuffer<glm::vec3>    gpu_mesh_positions;
    cgtub::GLAttributeBuffer<glm::u32vec3> gpu_mesh_indices;
    cgtub::GLAttributeBuffer<glm::vec3>    gpu_mesh_normals;
    cgtub::GLAttributeBuffer<glm::vec2>    gpu_mesh_uvs;

    

    // Upload data to OpenGL buffers (on the GPU).
    gpu_mesh_positions.upload(mesh_positions);
    gpu_mesh_indices.upload(mesh_indices);
    gpu_mesh_normals.upload(mesh_normals);
    gpu_mesh_uvs.upload(mesh_uvs);

    // Load a texture from the specified file.
    std::vector<glm::vec3> texture_data;
    int                    texture_width;
    int                    texture_height;
    ex4::load_texture_from_file(ex4::TextureType::Checker, &texture_data, &texture_width, &texture_height);

    // Upload texture data to a OpenGL texture (on the GPU).
    cgtub::GLTextureBuffer gpu_texture;
    gpu_texture.upload(texture_data, texture_width, texture_height);

    // Application data
    ex4::ShaderType   shader_type        = ex4::ShaderType::Ambient;
    ex4::TextureType  texture_type       = ex4::TextureType::Checker;
    ex4::GeometryType geometry_type      = ex4::GeometryType::Cube;
    glm::vec3         scale              = glm::vec3(1.f);
    glm::vec3         ambient_color      = glm::vec3(.25f);
    glm::vec3         diffuse_color      = glm::vec3(.5f);
    glm::vec3         specular_color     = glm::vec3(1.f);
    float             specular_magnitude = 64.f;
    glm::vec3         light_color        = glm::vec3(1.f);
    float             light_intensity    = 1.f;
    glm::vec3         light_position     = glm::vec3(1.f, 0.f, 0.f);
    glm::vec3         camera_position    = glm::vec3(glm::inverse(camera.view())[3]);

    float time = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window))
    {
        // Track current time and elapsed time between frames (dt)
        float now = static_cast<float>(glfwGetTime());
        float dt  = now - time;
        time      = now;

        cgtub::begin_frame(window);

        // Poll window and record window events (resizing, key inputs, etc.)
        // The dispatcher is implicitly connected to the window and receives these events.
        dispatcher->poll_window_events();

        // The canvas and renderer must react to incoming events (resizing, user inputs, ...)
        canvas.update(dt, dispatcher);
        controller.update(dt, dispatcher);

        ex4::GuiChanges changes = ex4::gui(&shader_type, &texture_type, &geometry_type, &scale,
                                           &ambient_color,
                                           &diffuse_color,
                                           &specular_color, &specular_magnitude,
                                           &light_color, &light_intensity, &light_position);

        if (ex4::has_gui_changed_parameter(changes, 0))
        {
            // Load the selected shader (TODO)
            std::string vertex_shader_path_ambient     = "shaders/ambient.v.glsl";
            std::string fragment_shader_path_ambient   = "shaders/ambient.f.glsl";
            std::string vertex_shader_path_normal      = "shaders/normal.v.glsl";
            std::string fragment_shader_path_normal    = "shaders/normal.f.glsl";
            std::string vertex_shader_path_lambert     = "shaders/lambert.v.glsl";
            std::string fragment_shader_path_lambert   = "shaders/lambert.f.glsl";
            std::string vertex_shader_path_phong       = "shaders/phong.v.glsl";
            std::string fragment_shader_path_phong     = "shaders/phong.f.glsl";
            std::string vertex_shader_path_uv          = "shaders/uv.v.glsl";
            std::string fragment_shader_path_uv        = "shaders/uv.f.glsl";
            std::string vertex_shader_path_spherical   = "shaders/spherical.v.glsl";
            std::string fragment_shader_path_spherical = "shaders/spherical.f.glsl";
            std::string vertex_shader_path_env         = "shaders/env.v.glsl";
            std::string fragment_shader_path_env       = "shaders/env.f.glsl";

            // Read shader from file
            if (shader_type == ex4::ShaderType::Ambient)
            {
                vertex_shader_code   = ex4::load_shader_from_file(vertex_shader_path_ambient);
                fragment_shader_code = ex4::load_shader_from_file(fragment_shader_path_ambient);
            }
            else if (shader_type == ex4::ShaderType::Normal)
            {
                vertex_shader_code   = ex4::load_shader_from_file(vertex_shader_path_normal);
                fragment_shader_code = ex4::load_shader_from_file(fragment_shader_path_normal);
            }
            else if (shader_type == ex4::ShaderType::Lambert)
            {
                vertex_shader_code = ex4::load_shader_from_file(vertex_shader_path_lambert);
                fragment_shader_code = ex4::load_shader_from_file(fragment_shader_path_lambert);
            }
            else if (shader_type == ex4::ShaderType::Phong)
            {
                vertex_shader_code   = ex4::load_shader_from_file(vertex_shader_path_phong);
                fragment_shader_code = ex4::load_shader_from_file(fragment_shader_path_phong); 
            }
            else if (shader_type == ex4::ShaderType::UV)
            {
                vertex_shader_code   = ex4::load_shader_from_file(vertex_shader_path_uv);
                fragment_shader_code = ex4::load_shader_from_file(fragment_shader_path_uv);
            }
            else if (shader_type == ex4::ShaderType::Spherical)
            {
                vertex_shader_code   = ex4::load_shader_from_file(vertex_shader_path_spherical);
                fragment_shader_code = ex4::load_shader_from_file(fragment_shader_path_spherical);
            }
            else if (shader_type == ex4::ShaderType::Environment)
            {
                vertex_shader_code   = ex4::load_shader_from_file(vertex_shader_path_env);
                fragment_shader_code = ex4::load_shader_from_file(fragment_shader_path_env);
            }
            // Compile program
            if (!pipeline.set_shaders(vertex_shader_code, fragment_shader_code))
                return EXIT_FAILURE;
        }

        if (ex4::has_gui_changed_parameter(changes, 1))
        {
            // Upload texture data to GPU texture
            ex4::load_texture_from_file(texture_type, &texture_data, &texture_width, &texture_height);
            gpu_texture.upload(texture_data, texture_width, texture_height);
        }

        if (ex4::has_gui_changed_parameter(changes, 2))
        {
            // Create the geometry on the CPU
            switch (geometry_type)
            {
            case ex4::GeometryType::Cube:
            {
                cgtub::create_box_geometry(glm::vec3(.3f), &mesh_positions, &mesh_indices, &mesh_normals, &mesh_uvs);
                break;
            }
            case ex4::GeometryType::Torus:
            {
                cgtub::create_torus_geometry(32, 32, glm::vec3(.15f), glm::vec3(.3f), &mesh_positions, &mesh_indices, &mesh_normals, &mesh_uvs);
                break;
            }
            case ex4::GeometryType::Sphere:
            default:
            {
                cgtub::create_sphere_geometry(32, 32, glm::vec3(.3f), &mesh_positions, &mesh_indices, &mesh_normals, &mesh_uvs);
                break;
            }
            }

            // Upload mesh data to GPU buffer
            gpu_mesh_positions.upload(mesh_positions);
            gpu_mesh_indices.upload(mesh_indices);
            gpu_mesh_normals.upload(mesh_normals);
            gpu_mesh_uvs.upload(mesh_uvs);
        }

        // Build the model matrix with scale from GUI
        glm::mat4 model_matrix = glm::scale(glm::mat4(1.0f), scale);

        // Common uniforms
        pipeline.set_uniform("model_matrix", model_matrix);
        pipeline.set_uniform("view_matrix", camera.view());
        pipeline.set_uniform("projection_matrix", camera.projection());

        // Common attribute
        pipeline.bind_attribute("position_vs", gpu_mesh_positions);

        // Shader specific uniforms and attributes
        if (shader_type == ex4::ShaderType::Ambient)
        {
            pipeline.set_uniform("ambient_color", ambient_color);
        }
        else if (shader_type == ex4::ShaderType::Normal)
        {
            pipeline.set_uniform("normal_matrix", glm::transpose(glm::inverse(glm::mat3(model_matrix))));
            pipeline.bind_attribute("normal_vs", gpu_mesh_normals);
        }
        else if (shader_type == ex4::ShaderType::Lambert)
        {
            pipeline.set_uniform("normal_matrix", glm::transpose(glm::inverse(glm::mat3(model_matrix))));
            pipeline.set_uniform("ambient_color", ambient_color);
            pipeline.set_uniform("diffuse_color", diffuse_color);
            pipeline.set_uniform("light_color", light_color);
            pipeline.set_uniform("light_intensity", light_intensity);
            pipeline.set_uniform("light_position", light_position);
            pipeline.bind_attribute("normal_vs", gpu_mesh_normals);
        }
        else if (shader_type == ex4::ShaderType::Phong)
        {
            pipeline.set_uniform("normal_matrix", glm::transpose(glm::inverse(glm::mat3(model_matrix))));
            pipeline.set_uniform("ambient_color", ambient_color);
            pipeline.set_uniform("diffuse_color", diffuse_color);
            pipeline.set_uniform("specular_color", specular_color);
            pipeline.set_uniform("specular_magnitude", specular_magnitude);
            pipeline.set_uniform("light_color", light_color);
            pipeline.set_uniform("light_intensity", light_intensity);
            pipeline.set_uniform("light_position", light_position);

            glm::vec3 camera_position = glm::vec3(glm::inverse(camera.view())[3]);
            pipeline.set_uniform("camera_position", camera_position);

            pipeline.bind_attribute("normal_vs", gpu_mesh_normals);
        }
        else if (shader_type == ex4::ShaderType::UV)
        {
            pipeline.bind_attribute("uvs_vs", gpu_mesh_uvs);
            pipeline.bind_texture("img_texture", gpu_texture);
        }
        else if (shader_type == ex4::ShaderType::Spherical)
        {
            pipeline.bind_texture("img_texture", gpu_texture);
        }
        else if (shader_type == ex4::ShaderType::Environment)
        {
            pipeline.set_uniform("normal_matrix", glm::transpose(glm::inverse(glm::mat3(model_matrix))));

            glm::vec3 camera_position = glm::vec3(glm::inverse(camera.view())[3]);
            pipeline.set_uniform("camera_position", camera_position);

            pipeline.bind_attribute("normal_vs", gpu_mesh_normals);
            pipeline.bind_texture("img_texture", gpu_texture);
        }

        canvas.clear();

        // Render the mesh using the index array
        pipeline.render_indexed(gpu_mesh_indices, canvas.viewport());

        cgtub::end_frame(window);
    }

    cgtub::uninit(window, dispatcher);

    return EXIT_SUCCESS;
}