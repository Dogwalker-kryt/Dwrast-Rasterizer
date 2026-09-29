# Dwrast Software Rasterizer

A lightweight CPU-based rasterizer built in C++17 as a learning and expiremnt project
This project is intentionally small and educational, but it is structured in a way that makes it easy to extend with texturing, perspective-correct interpolation, depth buffering, and more advanced rendering features.

## Features

- CPU-side rasterization **(WIP)**
- Custom framebuffer abstraction with pixel writes and clearing
- Geometry utilities for vectors and triangle math
- GTK4 window integration for live rendering previews
- PPM export support for static image output
- Resolution override via command-line arguments
- CMake + Ninja build setup

## Project status

This is an early-stage rendering project and a solid foundation for experimentation and learning. It is suitable for:

- looking at it
- use for experimenting in personal projects

## Requirements

Before building the project, make sure you have the following installed:

- CMake 3.20 or newer
- Ninja build system
- A C++17-compatible compiler (GCC or Clang)
- GTK4 development files
- pkg-config
- Linux environment

On Debian/Ubuntu-based systems, the GTK dependency can usually be installed with:

```bash
sudo apt-get install build-essential cmake ninja-build pkg-config libgtk-4-dev
```

## Build instructions

```bash
git clone https://github.com/Dogwalker-kryt/cpu-rasterizer.git
cd cpu-rasterizer
cmake -S . -B build -G Ninja
cmake --build build
```

This generates the executable at:

```bash
./build/cpu_rasterizer
```

## Usage

The application supports a few simple command-line flags:

- `-gtk` — opens a GTK window and renders in a windowed preview
- `-ppm <filename>` — writes the framebuffer to a PPM file
- `-width <value>`, `-w <value>` — sets the framebuffer width
- `-heigth <value>`, `-h <value>` — sets the framebuffer height

Examples:

```bash
./build/dwrast_rasterizer -gtk
```

```bash
./build/dwrast_rasterizer -ppm test.ppm -width 1280 -height 720
```

```bash
./build/dwrast_rasterizer -gtk -ppm output.ppm -width 800 -height 600
```

## How it works

The renderer is built around a small software pipeline:

1. A framebuffer is allocated for pixel storage.
2. Geometric primitives are defined as vectors and triangles.
3. Triangle edges are tested using a standard edge-function approach.
4. Pixels inside the triangle are filled based on the rasterization rules.
5. The image is either displayed with GTK or exported as a PPM image.

This is a foundational software rendering structure and is intentionally simple enough to understand and extend.

## Repository layout

```text
cpu-rasterizer/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── application.hpp
│   ├── main.cpp
│   ├── gtkui/
│   │   ├── ui.cpp
│   │   └── ui.hpp
│   ├── rasterizer/
│   │   ├── Framebuffer/
│   │   │   └── Framebuffer.hpp
│   │   ├── math/
│   │   │   └── min_max.h
│   │   ├── vector/
│   │   │   ├── triangle.h
│   │   │   ├── vec.c
│   │   │   └── vec.h
│   │   ├── colors.h
│   │   ├── defs.h
│   │   ├── dwrast_rasterizer.hpp
│   │   └── ...
│   └── renderer/
│       ├── renderer.cpp
│       └── renderer.hpp
├── build/
├── test.ppm
└── ...
```

## Roadmap

- basicly a whole and fully functional rasterizer, that makes it useful 

## License

This project is licend under the 

## Contact

For questions, suggestions, or collaboration inquiries, open an issue in the repository or reach out through the project’s GitHub page.

---

A minimal CPU rasterizer is a great way to learn how graphics pipelines work at a low level. This project is a practical starting point for building a more advanced software renderer in the future.
