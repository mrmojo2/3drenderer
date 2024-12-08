# Simple 3D Renderer

A simple 3D rendering engine based on rasterization. This project demonstrates the fundamentals of 3D rendering, including transformations, projection, and shading, implemented from scratch.

## Features

This renderer is capable of loading and rendering 3D models stored in Wavefront OBJ format. It utilizes 3D transformation matrices to perform essential operations such as translation, rotation, and scaling, allowing for dynamic manipulation of objects in the scene.

Projection is handled through matrices that account for aspect ratio and field of view (FOV), ensuring accurate and visually appealing perspectives. The renderer also includes clipping and backface culling techniques to optimize performance and avoid rendering unnecessary geometry.

A Z-buffer is implemented to manage depth information, ensuring that objects are correctly layered based on their distance from the camera. Additionally, the renderer incorporates simple lighting and shading methods to enhance the visual realism of rendered scenes.

## Getting Started

### Requirements

- A C/C++ compiler (e.g., GCC, Clang)
- [SDL2](https://www.libsdl.org/) library for window management and rendering

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/mrmojo2/3drenderer.git
   cd 3drenderer
   ```

2. Build the project:
   ```bash
   make
   ```

3. Run the renderer:
   ```bash
   ./bin/renderer <model.obj>
   ```

   Replace `<model.obj>` with the path to your `.obj` file.

