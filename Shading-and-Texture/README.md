# Shading & Texture

This project is an **interactive OpenGL application** demonstrating **shading models** and **texture mapping**. I implemented multiple **custom shaders** from scratch, including:

- **Ambient** – basic global illumination  
- **Normal** – visualizes surface normals  
- **Lambert** – diffuse lighting  
- **Phong** – diffuse + specular highlights  

Textures include **Checker, Colors, Earth, Indoor, Lava**, applied to CPU-generated geometries: **Cube, Sphere, and Torus**.

Users can interactively modify:

- **Material properties:** ambient, diffuse, specular colors, and specular exponent  
- **Lighting parameters:** color, intensity, and position  
- **Scale** of the object  
- **Shader, texture, and geometry selection**  

via a **real-time GUI** powered by **ImGui**. The camera uses a **turntable controller** for smooth navigation.

All geometry is generated on the CPU, uploaded to **GPU attribute buffers**, and rendered using a **custom GLRenderPipeline** with indexed rendering and shader uniforms. The rendering computations—including lighting and shading—run **on the GPU**, fully leveraging hardware acceleration.

---

### Screenshot

![Rendered Scene](texture.png)  

*Example: Cube with Checker texture and Phong shading.*

---

## **🛠️ Building the Project**

The project uses **CMake** for building. Ensure you have **CMake version**  installed and a C++ compiler capable of compiling C++20 (e.g., GCC , Clang , or MSVC ).

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
