# 3D Graphics Pipeline

## Features

-  **OBJ Loader**: Parses geometry, texture coordinates, and normals from `.obj` files.
-  **Vertex Shader**: Transforms vertices from model space → world space → camera space → clip space.
-  **Camera System**: Custom camera transformation with configurable position and orientation.
-  **Perspective Projection**: Implemented using `glm::perspective`.
-  **Clipping**: Liang-Barsky algorithm applied in screen space to discard triangles outside the frustum.
-  **NDC Conversion**: Transforms clip space coordinates to normalized device coordinates (NDC).
-  **Rasterizer**: Triangle rasterization using edge functions and barycentric coordinates.
-  **Fragment Shader**:
  - Interpolates texture coordinates and normals per-pixel.
  - Applies Lambertian lighting model.
  - Supports texture mapping using `stb_image`.
-  **Texture Mapping**: Per-pixel color sampling from loaded images.
-  **Image Output**: Final framebuffer is saved as `output.png`.

## Pipeline Overview

1. **Model Loading**
   - Loads `.obj` files and textures from `models/` directory.
   - Vertices, normals, UVs, and face indices are parsed and stored.

2. **Vertex Processing**
   - Each vertex is transformed through:
     ```
     model → world → view (camera) → clip → NDC
     ```
   - Perspective projection is handled using GLM.

3. **Clipping**
   - Liang-Barsky algorithm is used to clip triangles against the view frustum.
   - Only visible parts of geometry are rasterized.

4. **Rasterization**
   - Triangles are converted into fragments using edge functions.
   - Barycentric coordinates are used to interpolate depth, UVs, and normals.

5. **Fragment Shading**
   - Per-fragment lighting is computed using surface normals and a simple light model.
   - Textures are sampled with UV interpolation.
   - Final color is written to the framebuffer.

6. **Output**
   - The framebuffer is saved as a PNG image `output.png`.

## Dependencies

- [GLM (OpenGL Mathematics)](https://github.com/g-truc/glm): For matrix/vector math and projection.
- [stb_image.h](https://github.com/nothings/stb): Header-only image loader for texture mapping.

## Building and Running

### Build and run
Run the executable in this format:
```bash
./run {position} {direction}
```
For example:
```bash
./run 0.0 -5.0 2.0 0.0 -0.2 -1.0
```
View the rendered output:
```bash
feh output.png
```
![Output](output.png)
