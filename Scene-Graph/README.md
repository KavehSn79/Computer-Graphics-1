# **Scene Graph**

This project implements a 3D application using C++ and OpenGL-related libraries (GLFW, GLAD, GLM) to render a hierarchical model of a human-like figure. The application allows users to interactively select individual parts of the figure (entities) and manipulate their geometric transformations (scale, rotation, translation) via a graphical user interface (GUI).


## **✨ Demonstration**
![Recording 2025-10-10 130951](https://github.com/user-attachments/assets/d4e4be8b-95ee-4133-a889-27bad249355e)


## **🚀 Prerequisites**

This project relies on several common C++ and graphics libraries. To build and run this code, you will need:

* **C++ Compiler:** A modern C++ compiler (e.g., GCC, Clang, MSVC) supporting C++17 or newer.  
* **CMake:** Version .  
* **OpenGL Libraries:**  
  * **GLFW:** For window and input handling.  
  * **GLAD:** For managing OpenGL function pointers.  
  * **GLM (OpenGL Mathematics):** For vector and matrix operations.  
 
* **ImGui:** The presence of ex1::gui strongly suggests the use of a GUI library like ImGui for the interactive transformation controls.

## **📦 Core Concepts**

### **1\. The Entity Class (Scene Graph Node)**

The Entity class is the fundamental building block of the application's scene graph. Each instance represents a part of the 3D figure (body, head, arm, etc.) and manages its position and orientation in 3D space.

| Field | Description |
| :---- | :---- |
| translate, rotation, scale | The entity's **local** transformations. |
| parent | A pointer to the parent Entity in the hierarchy. |
| children | A list of child Entity objects. |
| original\_vertices | The mesh geometry in the entity's *local* coordinate system. |
| vertices | The transformed mesh geometry, ready for rendering. |

### **2\. Hierarchical Transformations**

* **localTransform():** Computes the transformation matrix (TRS: Translation  Rotation  Scale) relative to the entity's parent.  
* **globalTransform():** Recursively computes the entity's final world-space transformation matrix by multiplying its localTransform with the globalTransform of all its ancestors (parents, grandparents, etc.). This ensures that child parts move when their parent moves (e.g., the hand moves when the arm rotates).  
* **compute\_tramsformation\_matrix():** Contains the manual implementation of the TRS matrix construction.

### **3\. The Humanoid Hierarchy**

The 3D figure is built using the following parent-child relationships:

* **body (Root)**  
  * head  
  * left\_arm  
    * left\_fist  
  * right\_arm  
    * right\_fist  
  * left\_leg  
    * left\_foot  
  * right\_leg  
    * right\_foot

## **🕹️ Usage and Interactivity**

The application provides the following interactive features:

1. **Object Selection:** Click directly on any part of the 3D figure to select it. The selected part's color will change (typically to yellow) and its transformation controls will appear in the GUI.  
2. **Transformation Control:** Use the GUI sliders (controlled by the external ex1::gui function) to adjust the **Scale**, **Rotation**, and **Translation** of the currently selected entity.  
3. **Local Axes Visualization:** A checkbox, typically labeled "Show Coordinate System," can be toggled to render the local coordinate axes (X: Red, Y: Green, Z: Blue) for the selected entity.  
4. **Reset:** A reset button (controlled by the reset boolean) restores the scale, rotation, and translation of all entities to their default, initial values.

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
