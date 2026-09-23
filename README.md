# CPU Rasterizer

A small CPU-driven rasterizer project built in C/C++ with SIMD-friendly vector math and a lightweight framebuffer/GTK UI shell.

This project is meant to be a practical, low-level rasterization playground: vector math first, triangle math second, then rasterization, then shading, then a UI layer to display results.

## Project goal

The goal is to build a software rasterizer that:

- works directly on the CPU,
- keeps math simple and explicit,
- uses SIMD where it helps,
- renders triangles into a framebuffer,
- can eventually support projection, transforms, shading, and a windowed viewer.

The current codebase is structured around a small math layer and a rasterizer shell, instead of a large monolithic renderer.

## Current state

The project already includes:

- vector types: vec2f_t, vec3f_t, vec4f_t
- SoA/ AoS conversion helpers
- arithmetic operations for add, sub, mul, div, lerp, scale
- triangle and vertex definitions
- framebuffer abstraction
- GTK-based app shell

What is still missing is the actual render pipeline steps:

- model/world/view/projection transforms
- viewport transform
- clipping and triangle setup
- barycentric interpolation
- scanline or edge-based rasterization
- fragment shading
- final framebuffer blit/display

In other words: the math layer is alive, but the renderer is not yet fully assembled.

## Repository layout

```text
cpu-rasterizer/
├── CMakeLists.txt
├── README.md
├── build/
├── src/
│   ├── main.cpp
│   ├── application.hpp
│   ├── gtkui/
│   └── rasterizer/
│       ├── cpu_rasterizer.hpp
│       ├── defs.h
│       ├── colors.h
│       ├── Framebuffer/
│       └── vector/
│           ├── vec.h
│           ├── vec.c
│           └── triangle.h
```

## Core math model

The math layer is intentionally small and explicit.

### vec4f_t

This is the main generic vector type for positions and colors.

- position: x, y, z, w
- color: r, g, b, a
- can be used for both geometry and color data

The project aliases color-like values using the `rgba_t` typedef.

### vertex_t

A vertex is simply:

- a position
- a color

```c
typedef struct vertex_t {
    vec4f_t pos, color;
} vertex_t;
```

This is enough for a very basic immediate-mode renderer: each vertex has transformed position and associated color.

### triangle_t

A triangle is just three vertices in order:

```c
typedef struct triangle_t {
    vertex_t v0, v1, v2;
} triangle_t;
```

That is the minimum representation you need for triangle rasterization.

## What you need for triangle_t and vertex_t before moving on

To move from the math layer into real rendering, you need the following core assumptions and data to be valid.

### 1. A vertex must have a valid position

For a triangle to render, each vertex needs a usable position in the same space as the others.

Minimum expected state:

- `v0.pos`, `v1.pos`, `v2.pos` all exist
- they are in the same coordinate system
- the triangle is in a consistent order (winding matters)

At the start, the simplest pipeline is:

- model space -> world space -> view space -> clip space -> NDC

Your vertex positions should eventually be transformed through that chain before rasterization.

### 2. A triangle needs winding and edge information

Triangles need to be oriented consistently.

The project already has helper logic like:

- `triangle_area2`
- `triangle_is_degen`

This tells you:

- if the triangle has a non-zero area,
- if it is degenerate,
- if you are looking at a valid front-facing triangle.

You need triangle winding to determine:

- front/back face
- culling behavior
- barycentric computation

For the next stage, a triangle should be treated as:

- a set of 3 screen-space positions,
- a valid winding order,
- something that can be rasterized with edge tests.

### 3. You need interpolation data, not just position

The current vertex definition is minimal but good enough to evolve.

At some point, each vertex will usually need more than just `pos` and `color`:

- UV coordinates
- normals
- tangent basis
- texture indices
- per-vertex attributes for shading

For now, `pos` and `color` are enough to get a basic triangle renderer working.

### 4. A triangle should be screen-space ready before rasterization

Before drawing, the triangle should be in this form:

- `v0.pos`, `v1.pos`, `v2.pos` in clip or NDC space
- then converted to screen space
- then rasterized with integer or float pixel coverage logic

This is the next milestone: transform -> perspective divide -> viewport mapping -> rasterization.

### 5. You need a clear render pipeline

The minimum working pipeline looks like this:

1. Define triangle data
2. Transform vertices
3. Clip if needed
4. Perspective divide
5. Map to viewport
6. Rasterize triangle
7. Interpolate attributes across pixels
8. Shade each pixel
9. Write to framebuffer

The current project already has the right building blocks for steps 1, 2, and 7, but it still needs the actual pipeline assembly.

## Minimum next-step requirements

To proceed to the actual rasterizer, you really only need these pieces:

### Required

- a list of vertices in model space
- a list of triangles referencing those vertices
- a transform matrix stack or a simple projection matrix
- a viewport transform
- a framebuffer with width/height
- a per-pixel color write path
- edge-based or barycentric rasterizer

### Useful next additions

- `triangle_area2` and edge tests
- barycentric interpolation helpers
- clipping against the view frustum
- depth buffer
- perspective-correct interpolation
- simple shader functions

## Build

This project uses CMake.

```bash
cmake -S . -B build
cmake --build build
```

You can run the binary from the build directory, or wire it into your own test harness while the renderer is being developed.

## Recommended next milestone

The best next step is not “make a full engine.” The best next step is:

1. define a few test triangles,
2. transform them into screen space,
3. rasterize them to a small framebuffer,
4. fill pixels with flat color,
5. confirm the winding and edge logic works,
6. then add gradients or interpolated color.

That gives you a clean path from vector math to actual pixels without getting stuck in abstraction.

## Design philosophy

This project is intentionally built like a low-level, understandable software renderer:

- vectors and triangles are explicit,
- math is close to the underlying rasterization operations,
- the structure is simple enough to debug,
- performance is a conscious part of the design without becoming premature optimization theatre.

That is a good base for a real CPU rasterizer.

## Bottom line

The current `vertex_t` and `triangle_t` are already close to the minimum representation needed for rasterization.

The next thing you need is not more complicated types — it is a proper pipeline:

- transform vertices,
- convert to screen space,
- rasterize triangles,
- write color to framebuffer.

Once that works, the rest of the renderer becomes much easier: shading, depth, texture sampling, and eventually a more complete pipeline.
