#include <array>
#include <cstdint>
#include <iostream>
#include <queue>
#include <span>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cgtub/camera.hpp>
#include <cgtub/camera_controller_turntable.hpp>
#include <cgtub/canvas.hpp>
#include <cgtub/event_dispatcher.hpp>
#include <cgtub/geometry.hpp>
#include <cgtub/gl_wrap.hpp>
#include <cgtub/ndc_renderer.hpp>
#include <cgtub/primitives.hpp>
#include <cgtub/simple_renderer.hpp>

#include "helper.hpp"


glm::mat4 build_matrix_V(glm::vec3& e, glm::vec3& t, glm::vec3& up_ref)
{
    // forward (camera's -z direction)
    glm::vec3 f = glm::normalize(e - t);

    // right
    glm::vec3 r = glm::normalize(glm::cross(up_ref, f));

    // up
    glm::vec3 u = glm::cross(f, r);

    glm::mat4 V(1.0f);
    V[0][0] = r.x;
    V[1][0] = r.y;
    V[2][0] = r.z;
    V[3][0] = -glm::dot(r, e);
    V[0][1] = u.x;
    V[1][1] = u.y;
    V[2][1] = u.z;
    V[3][1] = -glm::dot(u, e);
    V[0][2] = f.x;
    V[1][2] = f.y;
    V[2][2] = f.z;
    V[3][2] = -glm::dot(f, e);
    V[0][3] = 0.0f;
    V[1][3] = 0.0f;
    V[2][3] = 0.0f;
    V[3][3] = 1.0f;

    return V;
}

glm::mat4 build_matrix_P_ortho(float size, float znear, float zfar)
{
    float l = -size * 0.5f;
    float r = size * 0.5f;
    float b = -size * 0.5f;
    float t = size * 0.5f;
    float n = znear;
    float f = zfar;

    glm::mat4 P_ortho(0.0f);
    P_ortho[0].x = 2 / (r - l);
    P_ortho[1].y = 2 / (t - b);
    P_ortho[2].z = -2 / (f - n);
    P_ortho[3].x = -(r + l) / (r - l);
    P_ortho[3].y = -(t + b) / (t - b);
    P_ortho[3].z = -(f + n) / (f - n);
    P_ortho[3].w = 1;

    return P_ortho;
}



glm::mat4 build_matrix_P_perspective(float fov_y_deg, float znear, float zfar)
{
    float fov_y_rad = glm::radians(fov_y_deg);
    float f         = 1.0f / std::tan(fov_y_rad / 2.0f);

    glm::mat4 P(0.0f);
    P[0][0] = f;                                      // scale x
    P[1][1] = f;                                      // scale y
    P[2][2] = (zfar + znear) / (znear - zfar);        // map z
    P[2][3] = (2.0f * zfar * znear) / (znear - zfar); // map z
    P[3][2] = -1.0f;                                  // perspective divide
    // P[3][3] remains 0

    return P;
}




int main(int argc, char** argv)
{
    ////////////////////////////////////////////////////////////////
    // Initialization of display structures (window, canvas, renderer, ...)
    ////////////////////////////////////////////////////////////////

    // Create a GLFW window and an OpenGL context
    // The event dispatcher records incoming events for a window
    // (e.g. change of size or mouse cursor movement)
    GLFWwindow*             window     = nullptr;
    cgtub::EventDispatcher* dispatcher = nullptr;
    if (!cgtub::init(1600, 400, "CG1", &window, &dispatcher))
    {
        std::cerr << "Failed to initialize OpenGL window" << std::endl;
        return EXIT_FAILURE;
    }

    // A canvas is a (logical) subregion of a window, defined by a (normalized) extent. 
    // We create four canvas that cover one fourth of the window width.

    cgtub::Canvas canvas_left        = cgtub::Canvas(window, cgtub::Extent{.x = 0.00f, .y = 0.f, .width = 0.248f, .height = 1.f});
    cgtub::Canvas canvas_middle_view = cgtub::Canvas(window, cgtub::Extent{.x = 0.25f, .y = 0.f, .width = 0.248f, .height = 1.f});
    cgtub::Canvas canvas_middle_clip = cgtub::Canvas(window, cgtub::Extent{.x = 0.50f, .y = 0.f, .width = 0.248f, .height = 1.f});
    cgtub::Canvas canvas_right       = cgtub::Canvas(window, cgtub::Extent{.x = 0.75f, .y = 0.f, .width = 0.25f, .height = 1.f});

    // A renderer can display primitives to a canvas (and a window respectively)
    // Internally, it manages a camera (the way we look at the virtual scene)
    // Each renderer corresponds to a single canvas.

    cgtub::SimpleRenderer    renderer_left(canvas_left);
    cgtub::SimpleRenderer    renderer_middle_view(canvas_middle_view);
    cgtub::SimpleRenderer    renderer_middle_clip(canvas_middle_clip);

    // This renderer takes normalized device coordinates and renders them accordingly.
    cgtub::NDCRenderer       renderer_right(canvas_right);

    ////////////////////////////////////////////////////////////////
    // Initialization of data (geometry, application state, ...)
    ////////////////////////////////////////////////////////////////

    // Define the axis of the world coordinate system using
    // start and end points of the lines (all axis start in the origin)
    glm::vec3 coordinate_axes_start_end[] = {
        glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), // x-axis
        glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), // y-axis
        glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), // z-axis
    };

    glm::vec3 coordinate_axes_color[] = {
        glm::vec3(1, 0, 0), // (r,g,b) color for the x-axis (red)
        glm::vec3(0, 1, 0), // (r,g,b) color for the y-axis (green)
        glm::vec3(0, 0, 1), // (r,g,b) color for the z-axis (blue)
    };

    // Create the geometry of the stanford bunny
    std::vector<glm::vec3> bunny_vertices;
    std::vector<glm::u32vec3> bunny_indices;
    ex2::create_bunny_geometry(&bunny_vertices, &bunny_indices);
    glm::vec3 bunny_color(0.75); // color used for rendering the mesh

    // Create a simple Sphere 
    std::vector<glm::vec3> sphere_vertices;
    std::vector<glm::u32vec3> sphere_indices;
    cgtub::create_sphere_geometry(0.03f, &sphere_vertices, &sphere_indices);

    // Camera position
    float azimuth = 0.f;

    // Internal camera parameters
    float fov   = 60.f;
    float size  = 0.8f;
    float znear = 0.35f;
    float zfar  = 1.60f;

    glm::vec3 t(0, 0, 0);
    glm::vec3 u_ref(0, 1, 0);
    glm::mat4 I = glm::mat4(1.0f); 

    // You need this to switch between orthographic and perspective transformation
    ex2::TransformationType transformation_type = ex2::TransformationType::Orthographic;

    ////////////////////////////////////////////////////////////////
    // Main loop: one iteration is one frame
    ////////////////////////////////////////////////////////////////
    float time = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window))
    {
        cgtub::begin_frame(window);

        // Track current time and elapsed time between frames (dt)
        float now = static_cast<float>(glfwGetTime());
        float dt  = now - time;
        time      = now;

        // Poll and record window events (resizing, key inputs, etc.)
        // The dispatcher is implicitly connected to the window and receives these events.
        dispatcher->poll_window_events();

        // The canvases and renderers must react to incoming events (resizing, user inputs, ...)
        canvas_left.update(dt, dispatcher);
        canvas_middle_view.update(dt, dispatcher);
        canvas_middle_clip.update(dt, dispatcher);
        canvas_right.update(dt, dispatcher);
        renderer_left.update(dt, dispatcher);
        renderer_middle_view.update(dt, dispatcher);
        renderer_middle_clip.update(dt, dispatcher);

        // Update the state of these variables through the GUI
        ex2::gui(&azimuth, &fov, &size, &znear, &zfar, &transformation_type);

        // Compute the camera origin from azimuth
        glm::vec3 camera_origin = glm::vec3(
            std::sin(azimuth) * std::sin(glm::quarter_pi<float>()),
            std::cos(glm::quarter_pi<float>()),
            std::cos(azimuth) * std::sin(glm::quarter_pi<float>()));

        

        /////////////////////////////////////////
        // Render the geometry using the current application state
        /////////////////////////////////////////
        
        // Clear the full window (clears the gaps between canvases)
        cgtub::clear(window, 0.f, 0.f, 0.f, 1.f); 

        // Clear the state of the left-most canvas
        canvas_left.clear(glm::vec3(1.f, 1.f, 1.f));

        

        
        // Render the world coordinate system
        renderer_left.render_lines(coordinate_axes_start_end, coordinate_axes_color);

        // Render the bunny geometry in world space
        renderer_left.render_mesh(bunny_vertices, bunny_indices, bunny_color);
        glm::mat4 V             = build_matrix_V(camera_origin, t, u_ref);
        glm::mat4 P_ortho       = build_matrix_P_ortho(size, znear, zfar);
        glm::mat4 P_perspective = build_matrix_P_perspective(fov, znear, zfar);
        glm::mat4 projection;

        if (transformation_type == ex2::TransformationType::Orthographic)
        {
            projection = P_ortho;
        }
        else
        {
            projection = P_perspective;
        }

        ex2::render_camera(renderer_left, V, projection);
        // Clear the state of the middle-view canvas
        canvas_middle_view.clear(glm::vec3(1.f, 1.f, 1.f));
        // Transform and render world axes
        std::array<glm::vec3, 6> axes_view;
        for (int i = 0; i < 6; i++)
        {
            glm::vec4 p  = V * glm::vec4(coordinate_axes_start_end[i], 1.0f);
            axes_view[i] = glm::vec3(p) / p.w;
        }
        renderer_middle_view.render_lines(axes_view, coordinate_axes_color);

        // Transform and render bunny
        std::vector<glm::vec3> bunny_vertices_view;
        bunny_vertices_view.reserve(bunny_vertices.size());
        for (auto const& v : bunny_vertices)
        {
            glm::vec4 p = V * glm::vec4(v, 1.0f);
            bunny_vertices_view.push_back(glm::vec3(p) / p.w);
        }
        renderer_middle_view.render_mesh(bunny_vertices_view, bunny_indices, bunny_color);

        // Draw the camera at origin in its own view space
        ex2::render_camera(renderer_middle_view, glm::mat4(1.0f), projection);


        canvas_middle_clip.clear(glm::vec3(1.f, 1.f, 1.f));


        std::array<glm::vec3, 6> axes_clip;
        for (int i = 0; i < 6; i++)
        {
            glm::vec4 p = projection * V * glm::vec4(coordinate_axes_start_end[i], 1.0f);
            axes_clip[i] = glm::vec3(p) / p.w;
        }

        renderer_middle_clip.render_lines(axes_clip, coordinate_axes_color);

        std::vector<glm::vec3> bunny_vertices_clip;
        bunny_vertices_clip.reserve(bunny_vertices.size());
        for (auto const& v : bunny_vertices)
        {
            glm::vec4 p = projection * V * glm::vec4(v, 1.0f);
            bunny_vertices_clip.push_back(glm::vec3(p) / p.w);
        }
        renderer_middle_clip.render_mesh(bunny_vertices_clip, bunny_indices, bunny_color);

        // draw clip-space cube and camera at origin (center of the cube)
        ex2::render_camera(renderer_middle_clip, glm::mat4(1.0f), glm::mat4(-1.0f));



        // Clear the right canvas (screen-space)
        canvas_right.clear(glm::vec3(1.f, 1.f, 1.f));

        // ---- 1. Transform bunny to NDC ----
        std::vector<glm::vec4> bunny_vertices_ndc;
        bunny_vertices_ndc.reserve(bunny_vertices.size());
        for (auto const& v : bunny_vertices)
        {
            glm::vec4 p_clip = projection * V * glm::vec4(v, 1.0f);
            bunny_vertices_ndc.push_back(p_clip); // Orthographic: p_clip.w = 1
        }

        // Render bunny in screen-space
        renderer_right.render_mesh(bunny_vertices_ndc, bunny_indices, bunny_color);

        // ---- 2. Transform coordinate axes to NDC ----
        std::vector<glm::vec4> axes_ndc;
        axes_ndc.reserve(6);
        for (int i = 0; i < 6; ++i)
        {
            glm::vec4 p_clip = projection * V * glm::vec4(coordinate_axes_start_end[i], 1.0f);
            axes_ndc.push_back(p_clip);
        }

        // Render axes in screen-space
        renderer_right.render_lines(axes_ndc, coordinate_axes_color);
        
        // Render the camera position as a sphere in world space
        std::vector<glm::vec3> camera_origin_vertices = sphere_vertices;
        for (glm::vec3& v : camera_origin_vertices)
        {
            v += camera_origin;
        }
        renderer_left.render_mesh(camera_origin_vertices, sphere_indices, glm::vec3(1, 0, 1));
        
        cgtub::end_frame(window);
    }

    cgtub::uninit(window, dispatcher);

    return EXIT_SUCCESS;
}

