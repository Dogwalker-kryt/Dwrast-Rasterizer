# DwRast

**A small CPU software rasterizer built from scratch in C++17.**

DwRast is an in-progress graphics project for experimenting with the fundamentals of rasterization: turning triangle geometry into pixels, storing those pixels in a framebuffer, and viewing or exporting the result. It can render Wavefront OBJ models with a perspective camera, flat shading and a depth buffer, or a simple built-in triangle demo, with a GTK4 preview and binary PPM image output.

> **Status: work in progress.** This is an experimental learning project, not a complete 3D renderer or a stable graphics library. Interfaces and features may change.

## Current features

- CPU-side filled-triangle and line rasterization
- Edge-function point-in-triangle testing, including either triangle winding
- Framebuffer clipping for triangle fill bounds
- An AVX2-assisted triangle-fill path with per-pixel handling for partial blocks
- Depth buffer with an AVX2 depth-tested triangle fill
- Wavefront OBJ loading (`v`, `vt`, `vn`, `f` with `v`, `v/vt`, `v//vn`, `v/vt/vn` and negative indices; polygons are fan-triangulated)
- Perspective projection with an orbit camera; models are centered and scaled to fit the view automatically
- Two-sided flat shading per face, using OBJ vertex colors (`v x y z r g b`) when present
- Real-time GTK preview that spins the model and shows FPS and raster time, plus a headless `-bench` mode
- GTK4 window for displaying the framebuffer
- PPM (P6) output for inspecting rendered images with common image tools
- Adjustable framebuffer dimensions from the command line

## Current limitations

Triangles that cross the camera's near plane are skipped rather than clipped. Texture coordinates and normals are parsed but not used for rendering yet, materials (`mtllib`/`usemtl`) are ignored, and the camera can't be controlled interactively yet; it only spins automatically. Texture mapping and a stable public API are not implemented yet. The rasterizer is still being developed, so output and performance may change.

## Build requirements

- Linux (GTK4 is currently required by the build)
- CMake 3.20 or newer
- Ninja
- A C++17-capable compiler, such as GCC or Clang
- GTK4 development headers and `pkg-config`
- A CPU with AVX2 and FMA3 support

On Debian or Ubuntu, install the build dependencies with:

```sh
sudo apt install build-essential cmake ninja-build pkg-config libgtk-4-dev
```

## Build

```sh
git clone https://github.com/Dogwalker-kryt/cpu-rasterizer.git
cd cpu-rasterizer
make
```

The Makefile is a thin wrapper around CMake. You can also call CMake directly:

```sh
cmake -S . -B build -G Ninja
cmake --build build
```

Useful `make` targets:

| Target | Description |
| --- | --- |
| `make` | Configure (if needed) and build into `build/`. |
| `make clean` | Delete `build/` and `build-asan/`. |
| `make rebuild` | Clean, then build. |
| `make gtk` | Open the real-time preview of `MODEL` with the depth buffer. |
| `make bench` | Run the headless benchmark on `MODEL`. |
| `make models` | Generate the test models (torus, cube, 100k and 400k triangle knots) into `models/`. |
| `make asan` | Build with AddressSanitizer and UBSan into `build-asan/`. |

`make gtk` and `make bench` generate their test model if it doesn't exist yet. Variables: `MODEL` (default `models/knot_100k.obj`), `FRAMES` (default 200), `W`/`H` for the framebuffer size, and `ARGS` for extra flags:

```sh
make bench MODEL=models/knot_400k.obj W=1920 H=1080 FRAMES=100
make gtk MODEL=path/to/your.obj
```

The executable is `build/dwrast_rasterizer`.

The project is compiled with AVX2, FMA3, and `-march=native`; the resulting binary requires AVX2/FMA3 and is tuned for the machine used to build it. It may not run on older or different CPUs. For portable release binaries, add runtime CPU-feature dispatch or build for a documented CPU baseline in `CMakeLists.txt`.

## Run

Open the GTK preview using the default framebuffer size (1000 × 500):

```sh
./build/dwrast_rasterizer -gtk
```

Render at a custom size and save the framebuffer to a PPM file:

```sh
./build/dwrast_rasterizer -width 800 -height 600 -ppm output.ppm
```

Render an OBJ model with the depth buffer enabled:

```sh
./build/dwrast_rasterizer -obj model.obj -depth -ppm model.ppm
```

Open a real-time preview: a loaded model spins continuously and the window title shows FPS, the rasterizer's own time per frame, and the triangle count. Press Ctrl+C in the terminal or close the window to quit; a summary is printed on exit:

```sh
./build/dwrast_rasterizer -gtk -obj model.obj -depth
```

Benchmark without a window. This renders N frames during one full camera turn and prints average, median, min, max, and p99 frame times plus triangles per second:

```sh
./build/dwrast_rasterizer -obj model.obj -depth -bench 200
./build/dwrast_rasterizer -obj model.obj -depth -w 1920 -h 1080 -bench 100
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
| `-depth` | `-z` | Activate the depth buffer for the current session. |
| `-obj <path>` | — | Load and render a Wavefront .obj model instead of the demo. Use it with `-depth`, otherwise faces overlap incorrectly. |
| `-bench <frames>` | — | Render the given number of frames headlessly while the camera turns once, then print frame-time statistics. |

The GTK preview requires a graphical session. PPM output can be generated without opening a window. The output is a binary P6 pixmap and can be opened or converted with standard image utilities.

## Project layout

```text
scripts/
└── gen_test_models.py              # Generates the test OBJ models
src/
├── application.hpp                 # Camera, model drawing, demo scene and frame rendering
├── main.cpp                        # CLI parsing and application startup
├── gtkui/                          # GTK4 framebuffer preview and real-time loop
├── obj_parser/                     # Wavefront OBJ loader
└── rasterizer/
    ├── Framebuffer/                # Framebuffer storage and PPM writing
    ├── boundingbox/                # Triangle bounds
    ├── math/                       # Small math helpers and 4x4 matrices
    ├── vector/                     # Vector and triangle types/utilities
    └── dwrast_rasterizer.hpp       # Line and triangle rasterization
```

## Development direction

Potential next steps for the project include near-plane clipping, smooth (per-vertex normal) shading, color interpolation, an interactive camera in the GTK preview, and texture mapping. These are ideas for future work—not features available in the current build.

## Contributing

Bug reports, focused fixes, and small feature contributions are welcome. Please open an issue to discuss larger changes before submitting a pull request. Include build steps and a concise description of how a rendering change was tested.

## License

It's licensed under the GPL-3.0 licence

## Project links

- [Source repository](https://github.com/Dogwalker-kryt/dwrast-rasterizer)
- [Report a bug or request a feature](https://github.com/Dogwalker-kryt/dwrast-rasterizer/issues)
