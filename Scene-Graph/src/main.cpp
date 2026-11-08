#include <iostream>
#include <vector>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cgtub/canvas.hpp>
#include <cgtub/event_dispatcher.hpp>
#include <cgtub/geometry.hpp>
#include <cgtub/gl_wrap.hpp>
#include <cgtub/primitives.hpp>
#include <cgtub/simple_renderer.hpp>
#include<math.h>

#include "helper.hpp"

class Entity
{
public:
    glm::vec3                            translate         = {0.0f, 0.0f, 0.0f};
    glm::vec3                            rotation          = {0.0f, 0.0f, 0.0f};
    glm::vec3                            scale             = {1.0f, 1.0f, 1.0f};
    glm::vec3                            default_translate = {0.0f, 0.0f, 0.0f};
    glm::vec3                            default_rotation  = {0.0f, 0.0f, 0.0f};
    glm::vec3                            default_scale     = {1.0f, 1.0f, 1.0f};
    glm::mat4                            localTransform();
    glm::mat4                            globalTransform();
    std::vector<std::shared_ptr<Entity>> children;
    std::shared_ptr<Entity>              parent = nullptr;
    std::vector<glm::vec3>               vertices;
    std::vector<glm::u32vec3>            indices;
    std::vector<glm::vec3>               original_vertices;
    int                                  id = -1;
    void                                 get_ready_to_render(glm::mat4);
    glm::mat4                            compute_tramsformation_matrix(glm::vec3, glm::vec3, glm::vec3);
    void                                 reset();

};

glm::mat4 Entity::localTransform()
{
    glm::mat4 local = glm::mat4(1.0f);
    local = compute_tramsformation_matrix(rotation, scale, translate);


    return local;
}

glm::mat4 Entity::compute_tramsformation_matrix(glm::vec3 rotation_, glm::vec3 scale_, glm::vec3 translate_)
{

    glm::mat4 R_x(
        1, 0, 0, 0,
        0, cos(rotation_.x), -sin(rotation_.x), 0,
        0, sin(rotation_.x), cos(rotation_.x), 0,
        0, 0, 0, 1);


    glm::mat4 R_y(
        cos(rotation_.y), 0, sin(rotation_.y), 0,
        0, 1, 0, 0,
        -sin(rotation_.y), 0, cos(rotation_.y), 0,
        0, 0, 0, 1);

    glm::mat4 R_z(
        cos(rotation_.z), -sin(rotation_.z), 0, 0,
        sin(rotation_.z), cos(rotation_.z), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    glm::mat4 R = R_y * R_x * R_z;

    glm::mat4 S = glm::mat4(1.0f);
    S[0].x      = scale_.x;
    S[1].y      = scale_.y;
    S[2].z      = scale_.z;

    glm::mat4 T = glm::mat4(1.0f);
    T[3].x      = translate_.x;
    T[3].y      = translate_.y;
    T[3].z      = translate_.z;

    glm::mat4 TRS = T * R * S;
    return TRS;
}

glm::mat4 Entity::globalTransform()
{
    glm::mat4 TRS;

    TRS = localTransform();

    std::shared_ptr<Entity> par = parent;

    while (par != nullptr)
    {
        glm::mat4 parent_transform = compute_tramsformation_matrix(par->rotation, par->scale, par->translate);
        TRS                        = parent_transform * TRS;
        par                        = par->parent;
    }

    return TRS;

}

void Entity::get_ready_to_render(glm::mat4 TRS)
{
    std::vector<glm::vec3> transformed_vertices;
    transformed_vertices.reserve(original_vertices.size());

    for (const auto& vertex : original_vertices)
    {
        glm::vec4 vertex_h      = glm::vec4(vertex, 1.0f);
        glm::vec4 transformed_h = TRS * vertex_h;
        transformed_vertices.push_back(glm::vec3(transformed_h));
    }

    vertices = transformed_vertices;
}

void Entity::reset()
{
    scale     = default_scale;
    rotation  = default_rotation;
    translate = default_translate;
}



std::vector<glm::vec3> get_start_end(const std::shared_ptr<Entity>& entity)
{
    glm::mat4 trs = entity->globalTransform();

    // Local axes in object space
    glm::vec4 origin(0, 0, 0, 1);
    glm::vec4 x_axis(1, 0, 0, 1);
    glm::vec4 y_axis(0, 1, 0, 1);
    glm::vec4 z_axis(0, 0, 1, 1);

    // Transform to world space
    glm::vec3 origin_w = glm::vec3(trs * origin);
    glm::vec3 x_axis_w = glm::vec3(trs * x_axis);
    glm::vec3 y_axis_w = glm::vec3(trs * y_axis);
    glm::vec3 z_axis_w = glm::vec3(trs * z_axis);

    std::vector<glm::vec3> start_end = {
        origin_w, x_axis_w, // x-axis
        origin_w, y_axis_w, // y-axis
        origin_w, z_axis_w  // z-axis
    };
    return start_end;
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
    if (!cgtub::init(640, 480, "CG1", &window, &dispatcher))
    {
        std::cerr << "Failed to initialize OpenGL window" << std::endl;
        return EXIT_FAILURE;
    }

    // A canvas is a (logical) subregion of a window, defined by a (normalized) extent.
    cgtub::Canvas canvas(window, cgtub::Extent{.x = 0.f, .y = 0.f, .width = 1.f, .height = 1.f});

    // A renderer can display primitives to a canvas (and a window respectively)
    // Internally, it manages a camera (the way we look at the virtual scene)
    cgtub::SimpleRenderer renderer(canvas);


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

    // Create a body and give it a unique id.
    int body_id = 0;

    // ... the body has a mesh.
    //     The box's vertices are translated and scaled to give it its unique shape.
    std::vector<glm::vec3>    body_mesh_vertices;
    std::vector<glm::u32vec3> body_mesh_indices;
    cgtub::create_box_geometry(1.f, &body_mesh_vertices, &body_mesh_indices);
    for (glm::vec3& v : body_mesh_vertices)
    {
        v *= glm::vec3(0.3f, 0.5f, 0.1f);
        v += glm::vec3(0.0f, 0.15f, 0.0f);
    }

    // ... and the body has a scale, rotation, and translation
    glm::vec3 body_scale(1, 1, 1);
    glm::vec3 body_rotation(0, 0, 0);
    glm::vec3 body_translation(0, 0, 0);


    // Application state
    int  selected_id            = -1;
    bool show_coordinate_system = false;

    auto body               = std::make_shared<Entity>();
    body->id                = 0;
    body->scale             = glm::vec3(1, 1, 1);
    body->rotation          = glm::vec3(0, 0, 0);
    body->translate         = glm::vec3(0, 0, 0);
    body->default_scale     = body->scale;
    body->default_rotation  = body->rotation;
    body->default_translate = body->translate;
    body->vertices          = body_mesh_vertices;
    body->indices           = body_mesh_indices;
    body->original_vertices = body_mesh_vertices;

    std::vector<glm::vec3>    head_mesh_vertices;
    std::vector<glm::u32vec3> head_mesh_indices;
    cgtub::create_sphere_geometry(0.25f, &head_mesh_vertices, &head_mesh_indices);
    


    auto head               = std::make_shared<Entity>();
    head->id                = 1;
    head->scale             = glm::vec3(1, 1, 1);
    head->rotation          = glm::vec3(0, 0, 0);
    head->translate         = glm::vec3(0, 1, 0); // place it above body
    head->default_scale     = head->scale;
    head->default_rotation  = head->rotation;
    head->default_translate = head->translate;
    head->vertices          = head_mesh_vertices;
    head->indices           = head_mesh_indices;
    head->original_vertices = head_mesh_vertices;

    head->parent = body;
    body->children.push_back(head);
    
    std::vector<glm::vec3>    left_arm_mesh_vertices;
    std::vector<glm::u32vec3> left_arm_mesh_indices;
    cgtub::create_box_geometry(1.f, &left_arm_mesh_vertices, &left_arm_mesh_indices);

    for (glm::vec3& v : left_arm_mesh_vertices)
    {
        v *= glm::vec3(0.35f, 0.1f, 0.1f); // long along X (arm points sideways)
        v += glm::vec3(0.35f, 0.0f, 0.0f); // shift so left end is at origin (shoulder joint)
    }
    


    auto left_arm               = std::make_shared<Entity>();
    left_arm->id                = 2;
    left_arm->scale             = glm::vec3(1, 1, 1);
    left_arm->rotation          = glm::vec3(0, 0, 0);
    left_arm->translate         = glm::vec3(0.35f, 0.55f, 0); // already positioned via vertices
    left_arm->default_scale     = left_arm->scale;
    left_arm->default_rotation  = left_arm->rotation;
    left_arm->default_translate = left_arm->translate;
    left_arm->vertices          = left_arm_mesh_vertices;
    left_arm->indices           = left_arm_mesh_indices;
    left_arm->original_vertices = left_arm_mesh_vertices;

    // attach to body
    left_arm->parent = body;
    body->children.push_back(left_arm);
    
    std::vector<glm::vec3>    left_fist_mesh_vertices;
    std::vector<glm::u32vec3> left_fist_mesh_indices;
    cgtub::create_box_geometry(1.f, &left_fist_mesh_vertices, &left_fist_mesh_indices);

    for (glm::vec3& v : left_fist_mesh_vertices)
    {
        v *= glm::vec3(0.1f, 0.1f, 0.1f); 
        v += glm::vec3(0.1f, 0.0f, 0.0f); 
    }

    auto left_fist               = std::make_shared<Entity>();
    left_fist->id                = 3;
    left_fist->scale             = glm::vec3(1, 1, 1);
    left_fist->rotation          = glm::vec3(0, 0, 0);
    left_fist->translate         = glm::vec3(0.75f, 0.0f, 0); // already positioned via vertices
    left_fist->default_scale     = left_fist->scale;
    left_fist->default_rotation  = left_fist->rotation;
    left_fist->default_translate = left_fist->translate;
    left_fist->vertices          = left_fist_mesh_vertices;
    left_fist->indices           = left_fist_mesh_indices;
    left_fist->original_vertices = left_fist_mesh_vertices;

    left_fist->parent = left_arm;
    left_arm->children.push_back(left_fist);

    std::vector<glm::vec3>    right_arm_mesh_vertices;
    std::vector<glm::u32vec3> right_arm_mesh_indices;
    cgtub::create_box_geometry(1.f, &right_arm_mesh_vertices, &right_arm_mesh_indices);

    for (glm::vec3& v : right_arm_mesh_vertices)
    {
        v *= glm::vec3(0.35f, 0.1f, 0.1f); // long along X (arm points sideways)
        v += glm::vec3(-0.35f, 0.0f, 0.0f); // shift so left end is at origin (shoulder joint)
    }

    auto right_arm               = std::make_shared<Entity>();
    right_arm->id                = 4;
    right_arm->scale             = glm::vec3(1, 1, 1);
    right_arm->rotation          = glm::vec3(0, 0, 0);
    right_arm->translate         = glm::vec3(-0.35f, 0.55f, 0); // already positioned via vertices
    right_arm->default_scale     = right_arm->scale;
    right_arm->default_rotation  = right_arm->rotation;
    right_arm->default_translate = right_arm->translate;
    right_arm->vertices          = right_arm_mesh_vertices;
    right_arm->indices           = right_arm_mesh_indices;
    right_arm->original_vertices = right_arm_mesh_vertices;

    // attach to body
    right_arm->parent = body;
    body->children.push_back(right_arm);

    std::vector<glm::vec3>    right_fist_mesh_vertices;
    std::vector<glm::u32vec3> right_fist_mesh_indices;
    cgtub::create_box_geometry(1.f, &right_fist_mesh_vertices, &right_fist_mesh_indices);

    for (glm::vec3& v : right_fist_mesh_vertices)
    {
        v *= glm::vec3(0.1f, 0.1f, 0.1f);
        v += glm::vec3(-0.1f, 0.0f, 0.0f);
    }

    auto right_fist               = std::make_shared<Entity>();
    right_fist->id                = 5;
    right_fist->scale             = glm::vec3(1, 1, 1);
    right_fist->rotation          = glm::vec3(0, 0, 0);
    right_fist->translate         = glm::vec3(-0.75f, 0.0f, 0); // already positioned via vertices
    right_fist->default_scale     = right_fist->scale;
    right_fist->default_rotation  = right_fist->rotation;
    right_fist->default_translate = right_fist->translate;
    right_fist->vertices          = right_fist_mesh_vertices;
    right_fist->indices           = right_fist_mesh_indices;
    right_fist->original_vertices = right_fist_mesh_vertices;

    right_fist->parent = right_arm;
    right_arm->children.push_back(right_fist);

    std::vector<glm::vec3>    left_leg_mesh_vertices;
    std::vector<glm::u32vec3> left_leg_mesh_indices;
    cgtub::create_box_geometry(1.f, &left_leg_mesh_vertices, &left_leg_mesh_indices);

    for (glm::vec3& v : left_leg_mesh_vertices)
    {
        v *= glm::vec3(0.1f, 0.3f, 0.1f);
        v += glm::vec3(0.0f, -0.3f, 0.0f);
    }

    auto left_leg               = std::make_shared<Entity>();
    left_leg->id                = 7;
    left_leg->scale             = glm::vec3(1, 1, 1);
    left_leg->rotation          = glm::vec3(0, 0, 0);
    left_leg->translate         = glm::vec3(0.2f, -0.4f, 0);
    left_leg->default_scale     = left_leg->scale;
    left_leg->default_rotation  = left_leg->rotation;
    left_leg->default_translate = left_leg->translate;
    left_leg->vertices          = left_leg_mesh_vertices;
    left_leg->indices           = left_leg_mesh_indices;
    left_leg->original_vertices = left_leg_mesh_vertices;

    left_leg->parent = body;
    body->children.push_back(left_leg);

    std::vector<glm::vec3>    left_foot_mesh_vertices;
    std::vector<glm::u32vec3> left_foot_mesh_indices;
    cgtub::create_box_geometry(1.f, &left_foot_mesh_vertices, &left_foot_mesh_indices);

    
    for (glm::vec3& v : left_foot_mesh_vertices)
    {
        v *= glm::vec3(0.1f, 0.05f, 0.08f);
        v += glm::vec3(0.0f, 0.05f, 0.0f); // shift so bottom face is at origin
    }


    auto left_foot               = std::make_shared<Entity>();
    left_foot->id                = 8;
    left_foot->scale             = glm::vec3(1, 1, 1);
    left_foot->rotation          = glm::vec3(0, 0, 0);
    left_foot->translate         = glm::vec3(0.0f, -0.6f, 0.22);
    left_foot->default_scale     = left_foot->scale;
    left_foot->default_rotation  = left_foot->rotation;
    left_foot->default_translate = left_foot->translate;
    left_foot->vertices          = left_foot_mesh_vertices;
    left_foot->indices           = left_foot_mesh_indices;
    left_foot->original_vertices = left_foot_mesh_vertices;
    
    left_foot->parent = left_leg;
    left_leg->children.push_back(left_foot);

    std::vector<glm::vec3>    right_leg_mesh_vertices;
    std::vector<glm::u32vec3> right_leg_mesh_indices;
    cgtub::create_box_geometry(1.f, &right_leg_mesh_vertices, &right_leg_mesh_indices);

    for (glm::vec3& v : right_leg_mesh_vertices)
    {
        v *= glm::vec3(0.1f, 0.3f, 0.1f);
        v += glm::vec3(0.0f, -0.3f, 0.0f);
    }

    auto right_leg               = std::make_shared<Entity>();
    right_leg->id                = 9;
    right_leg->scale             = glm::vec3(1, 1, 1);
    right_leg->rotation          = glm::vec3(0, 0, 0);
    right_leg->translate         = glm::vec3(-0.2f, -0.4f, 0);
    right_leg->default_scale     = right_leg->scale;
    right_leg->default_rotation  = right_leg->rotation;
    right_leg->default_translate = right_leg->translate;
    right_leg->vertices          = right_leg_mesh_vertices;
    right_leg->indices           = right_leg_mesh_indices;
    right_leg->original_vertices = right_leg_mesh_vertices;

    right_leg->parent = body;
    body->children.push_back(right_leg);

    std::vector<glm::vec3>    right_foot_mesh_vertices;
    std::vector<glm::u32vec3> right_foot_mesh_indices;
    cgtub::create_box_geometry(1.f, &right_foot_mesh_vertices, &right_foot_mesh_indices);

    for (glm::vec3& v : right_foot_mesh_vertices)
    {
        v *= glm::vec3(0.1f, 0.05f, 0.08f);
        v += glm::vec3(0.0f, 0.05f, 0.0f); // shift so bottom face is at origin
    }

    auto right_foot              = std::make_shared<Entity>();
    right_foot->id               = 10;
    right_foot->scale            = glm::vec3(1, 1, 1);
    right_foot->rotation         = glm::vec3(0, 0, 0);
    right_foot->translate        = glm::vec3(0.0f, -0.6f, 0.22);
    right_foot->default_scale     = right_foot->scale;
    right_foot->default_rotation  = right_foot->rotation;
    right_foot->default_translate = right_foot->translate;
    right_foot->vertices          = right_foot_mesh_vertices;
    right_foot->indices           = right_foot_mesh_indices;
    right_foot->original_vertices = right_foot_mesh_vertices;

    right_foot->parent = right_leg;
    right_leg->children.push_back(right_foot);

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

        // The canvas and renderer must react to incoming events (resizing, user inputs, ...)
        canvas.update(dt, dispatcher);
        renderer.update(dt, dispatcher);

        // Query if a user has clicked on one of the rendered meshes.
        // If the user clicks on the canvas, this call returns true and modifies `selected_id`.
        //   If the click "hits" a mesh, `selected_id` will be set to `id`, which has was
        //   passed when rendering this mesh with `renderer.render_mesh(..., id)`.
        //   If the click does not hit a mesh, `selected_id` will be -1.
        // If the user doesn't click on the canvas, this call returns false and doesn't modify `selected_id`.
        renderer.clicked(&selected_id);

        // Passing `nullptr` for a parameter hides the corresponding GUI element.
        bool reset = false;

        ex1::gui(
            selected_id == body->id ? &body->scale : selected_id == head->id    ? &head->scale
                                                 : selected_id == left_arm->id ? &left_arm->scale
                                                 : selected_id == right_arm->id  ? &right_arm->scale
                                                 : selected_id == left_fist->id ? &left_fist->scale
                                                 : selected_id == right_fist->id ? &right_fist->scale
                                                 : selected_id == left_leg->id ? &left_leg->scale
                                                 : selected_id == left_foot->id   ? &left_foot->scale
                                                 : selected_id == right_leg->id ? &right_leg->scale
                                                 : selected_id == right_foot->id  ? &right_foot->scale
                                                                                : nullptr,

            selected_id == body->id ? &body->rotation : selected_id == head->id    ? &head->rotation
                                                    : selected_id == left_arm->id ? &left_arm->rotation
                                                    : selected_id == left_fist->id ? &left_fist->rotation
                                                    : selected_id == right_arm->id  ? &right_arm->rotation
                                                    : selected_id == right_fist->id ? &right_fist->rotation
                                                    : selected_id == left_leg->id   ? &left_leg->rotation
                                                    : selected_id == left_foot->id  ? &left_foot->rotation
                                                    : selected_id == right_leg->id  ? &right_leg->rotation
                                                    : selected_id == right_foot->id ? &right_foot->rotation
                                                                                   : nullptr,

            selected_id == body->id ? &body->translate : selected_id == head->id    ? &head->translate
                                                     : selected_id == left_arm->id ? &left_arm->translate
                                                     : selected_id == left_fist->id ? &left_fist->translate
                                                     : selected_id == right_arm->id  ? &right_arm->translate
                                                     : selected_id == right_fist->id ? &right_fist->translate
                                                     : selected_id == left_leg->id   ? &left_leg->translate
                                                     : selected_id == left_foot->id  ? &left_foot->translate
                                                     : selected_id == right_leg->id  ? &right_leg->translate
                                                     : selected_id == right_foot->id ? &right_foot->translate
                                                                                    : nullptr,

            &show_coordinate_system,
            &reset);

        /////////////////////////////////////////
        // Render the geometry using the current application state
        /////////////////////////////////////////

        canvas.clear();
        glm::mat4 TRS_body       = body->globalTransform();
        glm::mat4 TRS_head       = head->globalTransform();
        glm::mat4 TRS_left_arm   = left_arm->globalTransform();
        glm::mat4 TRS_left_fist  = left_fist->globalTransform();
        glm::mat4 TRS_right_arm  = right_arm->globalTransform();
        glm::mat4 TRS_right_fist = right_fist->globalTransform();
        glm::mat4 TRS_left_leg   = left_leg->globalTransform();
        glm::mat4 TRS_left_foot  = left_foot->globalTransform();
        glm::mat4 TRS_right_leg  = right_leg->globalTransform();
        glm::mat4 TRS_right_foot = right_foot->globalTransform();


        body->get_ready_to_render(TRS_body);
        head->get_ready_to_render(TRS_head);
        left_arm->get_ready_to_render(TRS_left_arm);
        left_fist->get_ready_to_render(TRS_left_fist);
        right_arm->get_ready_to_render(TRS_right_arm);
        right_fist->get_ready_to_render(TRS_right_fist);
        left_leg->get_ready_to_render(TRS_left_leg);
        left_foot->get_ready_to_render(TRS_left_foot);
        right_leg->get_ready_to_render(TRS_right_leg);
        right_foot->get_ready_to_render(TRS_right_foot);

        glm::vec3 color             = selected_id == body->id ? glm::vec3(1, 1, 0) : glm::vec3(0.75);
        glm::vec3 head_color        = selected_id == head->id ? glm::vec3(1, 1, 0) : glm::vec3(0.75);
        glm::vec3 left_arm_color    = selected_id == left_arm->id ? glm::vec3(1, 1, 0) : glm::vec3(0.75);
        glm::vec3 left_fist_color   = selected_id == left_fist->id ? glm::vec3(1, 1, 0) : glm::vec3(0.75);
        glm::vec3 right_arm_color   = selected_id == right_arm->id ? glm::vec3(1, 1, 0) : glm::vec3(0.75);
        glm::vec3 right_fist_color  = selected_id == right_fist->id ? glm::vec3(1, 1, 0) : glm::vec3(0.75);
        glm::vec3 left_leg_color    = selected_id == left_leg->id ? glm::vec3(1, 1, 0) : glm::vec3(0.75);
        glm::vec3 left_foot_color   = selected_id == left_foot->id ? glm::vec3(1, 1, 0) : glm::vec3(0.75);
        glm::vec3 right_leg_color   = selected_id == right_leg->id ? glm::vec3(1, 1, 0) : glm::vec3(0.75);
        glm::vec3 right_foot_color  = selected_id == right_foot->id ? glm::vec3(1, 1, 0) : glm::vec3(0.75);

        renderer.render_mesh(body->vertices, body->indices, color, body->id);
        renderer.render_mesh(head->vertices, head->indices, head_color, head->id);
        renderer.render_mesh(left_arm->vertices, left_arm->indices, left_arm_color, left_arm->id);
        renderer.render_mesh(left_fist->vertices, left_fist->indices, left_fist_color, left_fist->id);
        renderer.render_mesh(right_arm->vertices, right_arm->indices, right_arm_color, right_arm->id);
        renderer.render_mesh(right_fist->vertices, right_fist->indices, right_fist_color, right_fist->id);
        renderer.render_mesh(left_leg->vertices, left_leg->indices, left_leg_color, left_leg->id);
        renderer.render_mesh(left_foot->vertices, left_foot->indices, left_foot_color, left_foot->id);
        renderer.render_mesh(right_leg->vertices, right_leg->indices, right_leg_color, right_leg->id);
        renderer.render_mesh(right_foot->vertices, right_foot->indices, right_foot_color, right_foot->id);

        if (show_coordinate_system)
            if (selected_id == body->id)
            {
                std::vector<glm::vec3> body_start_end = get_start_end(body);
                renderer.render_lines(body_start_end, coordinate_axes_color);
            }
            else if (selected_id == head->id)
            {
                std::vector<glm::vec3> head_start_end = get_start_end(head);
                renderer.render_lines(head_start_end, coordinate_axes_color);
            }
            else if (selected_id == left_arm->id)
            {
                std::vector<glm::vec3> left_arm_start_end = get_start_end(left_arm);
                renderer.render_lines(left_arm_start_end, coordinate_axes_color);
            }
            else if (selected_id == left_fist->id)
            {
                std::vector<glm::vec3> left_fist_start_end = get_start_end(left_fist);
                renderer.render_lines(left_fist_start_end, coordinate_axes_color);
            }
            else if (selected_id == right_arm->id)
            {
                std::vector<glm::vec3> right_arm_start_end = get_start_end(right_arm);
                renderer.render_lines(right_arm_start_end, coordinate_axes_color);
            }
            else if (selected_id == right_fist->id)
            {
                std::vector<glm::vec3> right_fist_start_end = get_start_end(right_fist);
                renderer.render_lines(right_fist_start_end, coordinate_axes_color);
            }
            else if (selected_id == left_leg->id)
            {
                std::vector<glm::vec3> left_leg_start_end = get_start_end(left_leg);
                renderer.render_lines(left_leg_start_end, coordinate_axes_color);
            }
            else if (selected_id == left_foot->id)
            {
                std::vector<glm::vec3> left_foot_start_end = get_start_end(left_foot);
                renderer.render_lines(left_foot_start_end, coordinate_axes_color);
            }
            else if (selected_id == right_leg->id)
            {
                std::vector<glm::vec3> right_leg_start_end = get_start_end(right_leg);
                renderer.render_lines(right_leg_start_end, coordinate_axes_color);
            }
            else if (selected_id == right_foot->id)
            {
                std::vector<glm::vec3> right_foot_start_end = get_start_end(right_foot);
                renderer.render_lines(right_foot_start_end, coordinate_axes_color);
            }
        if (reset)
        {
            body->reset();

            head->reset();

            left_arm->reset();

            left_fist->reset();

            right_arm->reset();

            right_fist->reset();

            left_leg->reset();

            left_foot->reset();

            right_leg->reset();

            right_foot->reset();
        }
                

        cgtub::end_frame(window);
    }

    cgtub::uninit(window, dispatcher);

    return EXIT_SUCCESS;
}