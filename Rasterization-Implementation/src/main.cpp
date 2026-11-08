#include <iostream>
#include <span>
#include <vector>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "imgui.h"

#include <algorithm>
#include <cgtub/camera_controller_turntable.hpp>
#include <cgtub/camera_perspective.hpp>
#include <cgtub/canvas.hpp>
#include <cgtub/event_dispatcher.hpp>
#include <cgtub/geometry.hpp>
#include <cgtub/gl_wrap.hpp>
#include <cgtub/image.hpp>
#include <cgtub/image_renderer.hpp>
#include <cgtub/primitives.hpp>
#include <cmath>

#include "helper.hpp"

void rasterize_lines(
    std::span<glm::vec4 const> points,
    std::span<glm::vec3 const> colors,
    int                        width,
    int                        height,
    std::vector<glm::vec3>*    image,
    std::vector<float>&        zbuffer,
    bool                       use_zbuffer,
    bool                       cull_behind_camera) // toggle z-buffer
{
    auto ndc_to_screen = [&](glm::vec4 const& p)
    {
        glm::vec4 ndc = p / p.w; // homogeneous divide
        float     x   = (ndc.x + 1.0f) * 0.5f * (width - 1);
        float     y   = (ndc.y + 1.0f) * 0.5f * (height - 1);
        return glm::vec2(x, y);
    };

    for (size_t i = 0; i < points.size(); i += 2)
    {
        glm::vec4 p0_ndc = points[i];
        glm::vec4 p1_ndc = points[i + 1];
        glm::vec3 color  = colors[i / 2];

        // Cull behind camera
        if (cull_behind_camera && (p0_ndc.w < 0 || p1_ndc.w < 0))
            continue;

        glm::vec2 p0 = ndc_to_screen(p0_ndc);
        glm::vec2 p1 = ndc_to_screen(p1_ndc);

        int x0 = (int)std::round(p0.x);
        int y0 = (int)std::round(p0.y);
        int x1 = (int)std::round(p1.x);
        int y1 = (int)std::round(p1.y);

        int dx  = std::abs(x1 - x0);
        int dy  = std::abs(y1 - y0);
        int sx  = (x0 < x1) ? 1 : -1;
        int sy  = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        int steps = std::max(dx, dy);
        int step  = 0;

        while (true)
        {
            float t = steps == 0 ? 0.0f : (float)step / (float)steps;

            // Interpolate z in NDC
            float z0 = p0_ndc.z / p0_ndc.w;
            float z1 = p1_ndc.z / p1_ndc.w;
            float z  = (1.0f - t) * z0 + t * z1;

            if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height && z >= -1.0f && z <= 1.0f)
            {
                int idx = y0 * width + x0;

                if (!use_zbuffer)
                {
                    // Draw without depth test
                    (*image)[idx] = color;
                }
                else
                {
                    // Draw only if closer
                    if (z < zbuffer[idx])
                    {
                        zbuffer[idx]  = z;
                        (*image)[idx] = color;
                    }
                }
            }

            if (x0 == x1 && y0 == y1)
                break;

            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y0 += sy;
            }

            step++;
        }
    }
}

void rasterize_mesh(
    std::span<glm::vec4 const>    positions,
    std::span<glm::u32vec3 const> indices,
    glm::vec3 const&              color,
    bool                          use_random_triangle_colors,
    int                           width,
    int                           height,
    std::vector<glm::vec3>*       image,
    std::vector<float>&           zbuffer,
    bool                          use_zbuffer,
    bool                          show_zbuffer,
    bool                          cull_behind_camera,
    bool                          cull_front_faces)
{
    auto ndc_to_screen = [&](glm::vec4 const& p)
    {
        glm::vec4 ndc = p / p.w; // homogeneous divide
        float     x   = (ndc.x + 1.0f) * 0.5f * (width - 1);
        float     y   = (ndc.y + 1.0f) * 0.5f * (height - 1);
        return glm::vec2(x, y);
    };

    for (size_t i = 0; i < indices.size(); ++i)
    {
        glm::u32vec3 tri = indices[i];

        glm::vec4 p0_ndc = positions[tri.x];
        glm::vec4 p1_ndc = positions[tri.y];
        glm::vec4 p2_ndc = positions[tri.z];

        // Cull behind camera
        if (cull_behind_camera)
        {
            if (p0_ndc.w < 0 || p1_ndc.w < 0 || p2_ndc.w < 0)
                continue; // skip triangle
        }

        glm::vec2 p0 = ndc_to_screen(p0_ndc);
        glm::vec2 p1 = ndc_to_screen(p1_ndc);
        glm::vec2 p2 = ndc_to_screen(p2_ndc);

        //  Frontface culling
        if (cull_front_faces)
        {
           
            float winding = (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
            if (winding > 0)
                continue;
        }

        glm::vec3 tri_color = use_random_triangle_colors ? ex3::get_random_color(i) : color;

        int xmin = std::max(0, (int)std::floor(std::min({p0.x, p1.x, p2.x})));
        int xmax = std::min(width - 1, (int)std::ceil(std::max({p0.x, p1.x, p2.x})));
        int ymin = std::max(0, (int)std::floor(std::min({p0.y, p1.y, p2.y})));
        int ymax = std::min(height - 1, (int)std::ceil(std::max({p0.y, p1.y, p2.y})));

        glm::vec2 v0    = p1 - p0;
        glm::vec2 v1    = p2 - p0;
        float     denom = v0.x * v1.y - v1.x * v0.y;
        if (std::abs(denom) < 1e-6f)
            continue; // skip degenerate triangles

        for (int y = ymin; y <= ymax; ++y)
        {
            for (int x = xmin; x <= xmax; ++x)
            {
                glm::vec2 v2 = glm::vec2(x + 0.5f, y + 0.5f) - p0;

                float w1 = (v2.x * v1.y - v1.x * v2.y) / denom; // weight for p1
                float w2 = (v0.x * v2.y - v2.x * v0.y) / denom; // weight for p2
                float w0 = 1.0f - w1 - w2;                      // weight for p0

                if (w0 >= 0 && w1 >= 0 && w2 >= 0)
                {
                    float z = w0 * (p0_ndc.z / p0_ndc.w) +
                              w1 * (p1_ndc.z / p1_ndc.w) +
                              w2 * (p2_ndc.z / p2_ndc.w);

                    if (z < -1.0f || z > 1.0f)
                        continue; // outside frustum

                    int idx = y * width + x;

                    if (!use_zbuffer)
                    {
                        if (!show_zbuffer)
                            (*image)[idx] = tri_color;
                    }
                    else if (z < zbuffer[idx])
                    {
                        zbuffer[idx] = z;
                        if (!show_zbuffer)
                            (*image)[idx] = tri_color;
                    }
                }
            }
        }
    }

    // Smooth z-buffer visualization
    if (show_zbuffer)
    {
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int   idx         = y * width + x;
                float z           = zbuffer[idx];
                float depth_color = 1.0f - (z + 1.0f) / 2.0f;
                depth_color       = glm::clamp(depth_color, 0.0f, 1.0f);
                (*image)[idx]     = glm::vec3(depth_color);
            }
        }
    }
}

int main(int argc, char** argv)
{
    // Create a GLFW window and an OpenGL context
    // The event dispatcher records incoming events for a window
    // (e.g. change of size or mouse cursor movement)
    GLFWwindow*             window     = nullptr;
    cgtub::EventDispatcher* dispatcher = nullptr;
    if (!cgtub::init(640, 480, "CG1", &window, &dispatcher))
    {
        std::cerr << "Failed to initialize OpenGL window" << std::endl;
        return EXIT_FAILURE;
    }

    // A canvas is a (logical) subregion of a window, defined by a (normalized) extent.
    cgtub::Canvas canvas(window, cgtub::Extent{.x = 0.f, .y = 0.f, .width = 1.f, .height = 1.f});

    // There is no more "SimpleRenderer" that manages the camera
    // but now the camera is explicitly accessible in the code.
    // The `camera` instance is only data representing a perspective camera ...
    cgtub::PerspectiveCamera camera(/*fov_y=*/45.f, /*aspect=*/1, /*z_near=*/1.f, /*z_far=*/4.5f);

    // ... while the `camera_controller` controls the camera.
    // This means, it processes user events and modifies
    // the camera parameters (i.e., rotating the camera if the user drags the mouse).
    cgtub::TurntableCameraController camera_controller(canvas, camera);

    // The rendering interface for this exercise is the `ImageRenderer`,
    // which can only render a canvas-filling image ...
    cgtub::ImageRenderer renderer(canvas);

    // ... so, in each frame, we want to generate image data
    // (i.e., a color for each pixel), which is then passed to the renderer.
    // The generated image should match the canvas aspect ratio,
    // so it's size (width, height) should be a multiple of the canvas'.
    cgtub::Rect viewport = canvas.viewport(true);
    int         width    = viewport.width;
    int         height   = viewport.height;

    // For large screen resolutions, the canvas resolution can be very
    // high and low-performance hardware may struggle to generate
    // the image data interactively. We therefore introduce the
    // `subsampling_rate` that reduces the number of pixels to generate.
    // If the `subsampling_rate` is 1, then `image` has as many pixels
    // as the canvas. If it is n, every pixel in `image` corresponds to
    // n^2 pixels on the canvas.
    int subsampling_rate = 4;
    width /= subsampling_rate;
    height /= subsampling_rate;

    // The image data itself (i.e. color for each pixel) is simply an array of colors.
    // For a pixel (x,y) the color is accessed as image[y*width + x].
    std::vector<glm::vec3> image(width * height, glm::vec3(0));

    // Create the scene geometry (a coordinate system, a box and a sphere)...
    glm::vec3 axes_start_end[] = {
        glm::vec3(0, 0, 0),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 1),
    };
    glm::vec3 axes_colors[] = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1),
    };

    std::vector<glm::vec3>    box_vertices;
    std::vector<glm::u32vec3> box_indices;
    cgtub::create_box_geometry(0.5f, &box_vertices, &box_indices);
    glm::vec3 box_color(1.f);

    std::vector<glm::vec3>    sphere_vertices;
    std::vector<glm::u32vec3> sphere_indices;
    cgtub::create_sphere_geometry(0.5f, &sphere_vertices, &sphere_indices);
    for (glm::vec3& v : sphere_vertices)
        v = v + glm::vec3(1, 0, 0);
    glm::vec3 sphere_color(0.f, 1.f, 0.f);

    // ...and reserve arrays for their NDC coordinates
    std::vector<glm::vec4> axes_start_end_ndc(std::size(axes_start_end));
    std::vector<glm::vec4> box_vertices_ndc(box_vertices.size());
    std::vector<glm::vec4> sphere_vertices_ndc(sphere_vertices.size());

    // Application state
    bool use_random_triangle_colors = false;
    bool use_z_buffer               = true;
    bool show_z_buffer              = false;
    bool cull_behind_camera         = false;
    bool cull_front_faces           = false;

    // Main loop: one iteration is one frame
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

        // The canvas and renderer must react to incoming events (resizing, user inputs, ...)
        canvas.update(dt, dispatcher);
        camera_controller.update(dt, dispatcher);

        ex3::GuiChanges gui_changes = ex3::gui(&subsampling_rate, &use_random_triangle_colors, &use_z_buffer, &show_z_buffer, &cull_behind_camera, &cull_front_faces);

        if (ex3::has_gui_changed_parameter(gui_changes, 0) || dispatcher->was_framebuffer_resized())
        {
            // If the window has been resized or subsampling was changed,
            // the image size needs to be adapted (unless it's zero)
            cgtub::Rect viewport = canvas.viewport(true);
            if (viewport.width != 0 && viewport.height != 0)
            {
                width  = viewport.width / subsampling_rate;
                height = viewport.height / subsampling_rate;
                image.resize(width * height);
            }
        }

        // Transform the coordinate axes, the box and the sphere to NDC
        glm::mat4 view_projection_matrix = camera.projection() * camera.view();
        for (size_t i = 0; i < std::size(axes_start_end); ++i)
            axes_start_end_ndc[i] = view_projection_matrix * glm::vec4(axes_start_end[i], 1.f);
        for (size_t i = 0; i < box_vertices.size(); ++i)
            box_vertices_ndc[i] = view_projection_matrix * glm::vec4(box_vertices[i], 1.f);
        for (size_t i = 0; i < sphere_vertices.size(); ++i)
            sphere_vertices_ndc[i] = view_projection_matrix * glm::vec4(sphere_vertices[i], 1.f);

        // Fill the image with a dummy color (here, one could clear the image with a constant color)
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                image[y * width + x] = glm::vec3(0.0f); // ex3::generate_dummy_color(x, y, width, height)
            }
        }

        bool use_zbuffer  = use_z_buffer;
        bool show_zbuffer = show_z_buffer;
        bool cull_behind  = cull_behind_camera;
        bool cull_front   = cull_front_faces;

        std::vector<float> zbuffer(width * height, 1.0f);
        // Clear image and z-buffer
        std::fill(image.begin(), image.end(), glm::vec3(0.0f));
        if (use_zbuffer)
            std::fill(zbuffer.begin(), zbuffer.end(), 1.0f);
        // Rasterize coordinate axes
        rasterize_lines(
            axes_start_end_ndc,
            axes_colors,
            width,
            height,
            &image,
            zbuffer,
            use_zbuffer,
            cull_behind);
        // Rasterize box and sphere
        rasterize_mesh(
            box_vertices_ndc,
            box_indices,
            box_color,
            use_random_triangle_colors,
            width,
            height,
            &image,
            zbuffer,
            use_zbuffer,
            show_zbuffer,
            cull_behind,
            cull_front);

        rasterize_mesh(
            sphere_vertices_ndc,
            sphere_indices,
            sphere_color,
            use_random_triangle_colors,
            width,
            height,
            &image,
            zbuffer,
            use_zbuffer,
            show_zbuffer,
            cull_behind,
            cull_front);

        // Display the generated image on the canvas
        // (don't need to clear the canvas because image fully fills it)
        renderer.render(image, width, height);

        cgtub::end_frame(window);
    }

    cgtub::uninit(window, dispatcher);

    return EXIT_SUCCESS;
}
