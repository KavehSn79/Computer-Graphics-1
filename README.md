# Computer Graphics 1 — TU Berlin

This repository contains my projects for the **Computer Graphics 1** course at **Technische Universität Berlin (TU Berlin)**.

The six projects cover scene graphs, coordinate transformations, software rasterization, shading and texture mapping, path tracing, and animation and simulation.

## Projects

| # | Project | Topics | Executable |
| --- | --- | --- | --- |
| 1 | [Scene Graph](Scene-Graph/) | Hierarchical modeling and interactive transformations | `ex1` |
| 2 | [3D Graphics Pipeline](3D-Graphics-Pipeline-Visualization-World-to-NDC-Transformations/) | World, view, clip, and normalized device coordinates | `ex2` |
| 3 | [Rasterization](Rasterization-Implementation/) | Line and triangle rasterization, depth testing, and culling | `ex3` |
| 4 | [Shading and Texture](Shading-and-Texture/) | Ambient, normal, Lambert, and Phong shading; texture mapping | `ex4` |
| 5 | [Path Tracing](Path-Tracing/) | Ray intersections and Monte Carlo global illumination | `ex5` |
| 6 | [Skeletal Animation and Cloth Simulation](Skeletal-Animation-Cloth-Simulation/) | Skeletal animation, spring systems, and cloth dynamics | `ex6` |

Each project is self-contained, with its original source code, assets, dependencies, and README.

## Building

The projects use **C++20**, **CMake 3.14 or newer**, and OpenGL-related libraries. Dependencies are included in each project's `ext/` directory. A compatible compiler and the platform's OpenGL/window-system development packages are required.

Build each project separately. For example, from the repository root:

```sh
cmake -S Scene-Graph -B Scene-Graph/build -DCMAKE_BUILD_TYPE=Release
cmake --build Scene-Graph/build --config Release --parallel
```

Run the generated `ex1` executable from the build output directory. Its location depends on the generator and platform; it may be under `Scene-Graph/build/Release/`.

Replace `Scene-Graph` with another project directory and use the corresponding executable listed above. See each project's README for its features, demonstrations, and controls.

## Acknowledgments

These projects were developed as coursework for Computer Graphics 1 at TU Berlin. The included course framework, third-party libraries, and assets retain their original notices and licenses.
