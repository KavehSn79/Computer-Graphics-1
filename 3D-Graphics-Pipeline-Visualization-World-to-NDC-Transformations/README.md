# **3D Graphics Pipeline Visualization: World to NDC Transformations**

This project is a C++ application designed to illustrate the initial stages of the fixed-function 3D graphics pipeline. It synchronously visualizes a 3D model (the Stanford Bunny) and its transformations across four side-by-side canvases, demonstrating the effects of the **View ()** and **Projection ()** matrices.

The application uses custom, manually-implemented matrix calculation functions for the View and Projection stages, providing a clear educational demonstration of how 3D coordinates are converted to screen space.

## **✨ Demonstration**

<img width="1597" height="396" alt="Screenshot 2025-10-10 162845" src="https://github.com/user-attachments/assets/5cc3463f-cb1b-4a95-9bc6-2ec6fd5f2bd4" />



## **🚀 Prerequisites**

This project is built using C++ and CMake, leveraging several popular graphics libraries.

* **C++ Compiler:** A modern C++ compiler supporting C++20 (e.g., GCC , Clang , MSVC ).  
* **CMake:** Version .  
* **Libraries:** **GLFW** (window/input), **GLAD** (OpenGL loading), **GLM** (vector/matrix math), and **ImGui** (for the interactive controls).

## **📦 Core Concepts: The Four Viewports**

The application utilizes four synchronized viewports to visualize the model at different stages of the transformation pipeline:

1. **Canvas 1 (Left): World Space ():**  
   * Displays the model (Stanford Bunny) and the world coordinate axes.  
   * The camera's viewing frustum (or cube for Orthographic) is rendered here relative to the world origin.  
   * **Transformation:** Identity matrix is applied.  
2. **Canvas 2 (Middle View): View Space ():**  
   * Displays the geometry after being transformed by the **View Matrix ()**.  
   * The camera is now located at the origin  looking down the negative Z-axis.  
   * **Transformation:**  is applied.  
3. **Canvas 3 (Middle Clip): Clip Space ():**  
   * Displays the geometry after being transformed by the combined **Projection ()** and **View ()** matrices.  
   * This view shows the space before the perspective divide, normalized into a  cube (the clipping volume).  
   * **Transformation:**  is applied.  
4. **Canvas 4 (Right): NDC/Screen Space:**  
   * Displays the geometry after the **Perspective Divide** (normalizing coordinates by ).  
   * The coordinates are now in **Normalized Device Coordinates (NDC)**, ranging from  to  for visible objects. This is the final 3D state before the rasterizer converts it to 2D pixels.  
   * **Transformation:**  is applied, followed by the perspective divide.

## **📐 Manual Matrix Implementations**

The core functionality relies on custom C++ functions for generating the fundamental transformation matrices without relying on GLM's built-in functions:

| Function | Matrix | Description |
| :---- | :---- | :---- |
| build\_matrix\_V() | View | Constructs the camera's View Matrix using the look-at configuration: eye position (), target position (), and up reference vector (). |
| build\_matrix\_P\_ortho() | Orthographic Projection | Constructs the Orthographic Projection Matrix based on size, znear, and zfar clipping planes. |
| build\_matrix\_P\_perspective() | Perspective Projection | Constructs the Perspective Projection Matrix based on the field-of-view (fov\_y\_deg), znear, and zfar clipping planes. |

## **🕹️ Usage and Interactivity**

The application features an interactive GUI (provided by an external helper library, likely ImGui) that allows the user to manipulate key camera parameters in real-time:

* **Azimuth Rotation:** Adjusts the rotation of the camera around the central target.  
* **FOV/Size:** Controls the field-of-view (for Perspective) or the size (for Orthographic).  
* **Clipping Planes:** Modifies znear and zfar to change the depth of the viewing volume, dynamically affecting the clipping in the second and third viewports.  
* **Projection Toggle:** Switches the active Projection Matrix between **Orthographic** and **Perspective**.

## **🛠️ Building the Project**

The project uses **CMake** for building. Ensure you have the required prerequisites installed.

### **Steps to Build and Run**

Run these commands in the root directory of the project:

**1\. Configure the project:**

cmake . \-B build \-DCMAKE\_BUILD\_TYPE=Release

This command creates a build directory and configures the project files.

**2\. Build the executable:**

cmake \--build build \--parallel \--config Release

This builds the project executables, placing them inside the build directory.

3\. Run the application:  
To Run the program:
./src/main  
\# or (example for Windows)  
.\\src\\main.exe

***Note: The exact path to the executable may vary based on your CMake setup.***
