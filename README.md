# DwRast

**A small CPU software rasterizer built from scratch in C++17.**

DwRast is an in-progress graphics project for experimenting with the fundamentals of rasterization: turning triangle geometry into pixels, storing those pixels in a framebuffer, and viewing or exporting the result. It currently renders a simple triangle demo, with a GTK4 preview and binary PPM image output.

> **Status: work in progress.** This is an experimental learning project, not a complete 3D renderer or a stable graphics library. Interfaces and features may change.

## Current features

- CPU-side filled-triangle and line rasterization
- Edge-function point-in-triangle testing, including either triangle winding
- Framebuffer clipping for triangle fill bounds
- An AVX2-assisted triangle-fill path with per-pixel handling for partial blocks
- GTK4 window for displaying the framebuffer
- PPM (P6) output for inspecting rendered images with common image tools
- Adjustable framebuffer dimensions from the command line

## Current limitations

The application currently draws a built-in demo; it does not load models or accept arbitrary scene geometry. A 3D camera/projection pipeline, depth buffer, texture mapping, and a stable public API are not implemented yet. The rasterizer is still being developed, so output and performance may change.

## Build requirements

- Linux (GTK4 is currently required by the build)
- CMake 3.20 or newer
- Ninja
- A C++17-capable compiler, such as GCC or Clang
- GTK4 development headers and `pkg-config`

On Debian or Ubuntu, install the build dependencies with:

```sh
sudo apt install build-essential cmake ninja-build pkg-config libgtk-4-dev
```

## Build

```sh
git clone https://github.com/Dogwalker-kryt/cpu-rasterizer.git
cd cpu-rasterizer
cmake -S . -B build -G Ninja
cmake --build build
```

The executable is `build/dwrast_rasterizer`.

The project is compiled with `-march=native`, so the resulting binary is tuned for the machine used to build it and may not run on a different CPU. For portable release binaries, revisit the target-specific compiler options in `CMakeLists.txt` and build/test for the intended CPU baseline.

## Run

Open the GTK preview using the default framebuffer size (1000 × 500):

```sh
./build/dwrast_rasterizer -gtk
```

Render at a custom size and save the framebuffer to a PPM file:

```sh
./build/dwrast_rasterizer -width 800 -height 600 -ppm output.ppm
```

Open the preview and save the same rendered image:

```sh
./build/dwrast_rasterizer -gtk -w 800 -h 600 -ppm output.ppm
```

### Command-line options

| Option | Alias | Description |
| --- | --- | --- |
| `-gtk` | — | Open the GTK framebuffer preview. |
| `-ppm <path>` | — | Write the rendered framebuffer to a PPM file. |
| `-width <pixels>` | `-w <pixels>` | Set the framebuffer width. |
| `-height <pixels>` | `-h <pixels>` | Set the framebuffer height. |

The GTK preview requires a graphical session. PPM output can be generated without opening a window. The output is a binary P6 pixmap and can be opened or converted with standard image utilities.

## Project layout

```text
src/
├── application.hpp                 # Demo scene and frame rendering
├── main.cpp                        # CLI parsing and application startup
├── gtkui/                          # GTK4 framebuffer preview
└── rasterizer/
    ├── Framebuffer/                # Framebuffer storage and PPM writing
    ├── boundingbox/                # Triangle bounds
    ├── math/                       # Small math helpers
    ├── vector/                     # Vector and triangle types/utilities
    └── dwrast_rasterizer.hpp       # Line and triangle rasterization
```

## Development direction

Potential next steps for the project include color interpolation, a depth buffer, a transform and camera pipeline, mesh loading, and texture mapping. These are ideas for future work—not features available in the current build.

## Contributing

Bug reports, focused fixes, and small feature contributions are welcome. Please open an issue to discuss larger changes before submitting a pull request. Include build steps and a concise description of how a rendering change was tested.

## License

It's licensed under the GPL-3.0 licence

## Project links

- [Source repository](https://github.com/Dogwalker-kryt/dwrast-rasterizer)
- [Report a bug or request a feature](https://github.com/Dogwalker-kryt/dwrast-rasterizer/issues)
