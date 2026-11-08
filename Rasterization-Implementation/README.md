# **Rasterization Implementation**

This C++ project implements a fundamental **Rasterization**, moving beyond hardware acceleration to manually simulate the crucial rasterization stage of the graphics pipeline. The application loads a 3D scene (a box and a sphere) and draws them onto a 2D image array (frame buffer) pixel by pixel, using custom functions for line and triangle drawing, depth testing, and various culling techniques.

This project is primarily focused on demonstrating the core algorithms responsible for converting 3D geometry into 2D pixels.

## **✨ Demonstration**

<img width="1081" height="613" alt="Screenshot 2025-10-10 165714" src="https://github.com/user-attachments/assets/498aa4b1-28a7-44c4-9820-2532fa6f4432" />


## **🚀 Prerequisites**

This project is built using C++ and CMake.

* **C++ Compiler:** A modern C++ compiler supporting C++20 (e.g., GCC , Clang , MSVC ).  
* **CMake:** Version .  
* **Libraries:** **GLFW** (window/input), **GLAD** (OpenGL loading), **GLM** (vector/matrix math), and **ImGui** (for the interactive controls).

## **📦 Core Concepts: Algorithms and Data Structures**

The rendering is performed entirely in software on the CPU, with the final image then uploaded to the GPU for display.

### **1\. Rasterization Algorithms**

| Function | Geometry | Algorithm | Description |
| :---- | :---- | :---- | :---- |
| rasterize\_lines() | Lines (Axes) | Digital Differential Analyzer (DDA) / Bresenham's | Draws lines by calculating integer pixel steps, and includes \-interpolation for depth testing. |
| rasterize\_mesh() | Triangles (Mesh) | Barycentric Coordinates | Fills triangles by checking if a pixel lies within the triangle boundary using barycentric coordinates . |

### **2\. Core Components**

* **Frame Buffer (std::vector\<glm::vec3\> image):** An array storing the color (RGB, ) for every pixel, representing the final output image.  
* **Z-Buffer (std::vector\<float\> zbuffer):** An array storing the minimum  (depth) value written to each pixel, used for depth testing. It is initialized to  (far plane).  
* **NDC to Screen Conversion:** The application first transforms 3D geometry into **Normalized Device Coordinates (NDC)** using the camera's  matrix, then converts NDC coordinates  to screen space coordinates  and  for rasterization.  
* **Subsampling:** The application uses a subsampling\_rate to intentionally reduce the rendered image resolution to improve performance, especially on higher-resolution displays.

### **3\. Rendering Techniques Implemented**

| Technique | Description |
| :---- | :---- |
| **Depth Test (Z-Buffering)** | Performed within both rasterize\_lines and rasterize\_mesh. A pixel is only drawn if its interpolated \-depth is closer (less than) the current value stored in the Z-Buffer at that pixel location. |
| **Backface Culling** | Controlled by cull\_front\_faces. Calculates the normal of a triangle face in View space and discards it if the normal is facing the camera, preventing rendering of hidden surfaces. |
| **Near-Plane Culling** | Controlled by cull\_behind\_camera. Discards vertices/triangles whose homogeneous coordinate  is negative, effectively performing an early cull for geometry behind the camera's near clipping plane. |
| **Z-Buffer Visualization** | Toggled by show\_z\_buffer. Instead of rendering color, the depth value  is visualized as a grayscale intensity (Near \= White, Far \= Black). |

## **🕹️ Usage and Interactivity**

The application features an interactive GUI that allows the user to toggle and visualize the effects of the core rasterization algorithms:

* **Subsampling Rate:** Adjusts the rendered image resolution for performance benchmarking.  
* **Use Z-Buffer:** Enables or disables depth testing.  
* **Show Z-Buffer:** Visualizes the depth values instead of the color image.  
* **Cull Behind Camera:** Toggles the near-plane culling check.  
* **Cull Front Faces:** Toggles backface culling (for demonstration purposes, front faces are culled in this implementation).  
* **Camera Control:** The scene can be rotated and zoomed using the mouse via the TurntableCameraController.

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
The primary executable is main.cpp :  
./src/main  
\# or (example for Windows)  
.\\src\\main.exe

