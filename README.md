# CPU Rasterizer

A high-performance CPU rasterizer written in C++17 utilizing SSE2 intrinsics for optimized vectorized operations. 
This project is designed to run efficiently on x86_64 architecture and is built using CMake 3.20+ and Ninja as the 
build system.

## Features

- **C++17**: Leveraging modern C++ features for clean and efficient code.
- **SSE2 Intrinsics**: Utilizing Streaming SIMD Extensions 2 for vectorized operations to enhance performance.
- **x86_64 Architecture**: Optimized for 64-bit Intel/AMD processors.
- **CMake Build System**: Facilitates cross-platform build configurations and dependencies management.
- **Ninja Build Tool**: Fast and lightweight build tool that integrates well with CMake.

## Prerequisites

To build and run the CPU Rasterizer, ensure you have the following installed:

- **CMake 3.20+**: Download and install from [CMake's official website](https://cmake.org/download/).
- **Ninja**: Download and install from [Ninja's official website](https://ninja-build.org/).
- **C++17 Compiler**: Ensure you have a C++17 compatible compiler installed (e.g., GCC, Clang).
- **Linux Environment**: The project is designed to run on Linux systems.

## Building the Project

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/Dogwalker-kryt/cpu-rasterizer.git
   cd cpu-rasterizer
   ```

2. **Create a Build Directory**:

   ```bash
   mkdir build
   cd build
   ```

3. **Configure the Project with CMake**:

   ```bash
   cmake -G Ninja ..
   ```

4. **Build the Project**:

   ```bash
   ninja
   ```

   This will compile the project and generate the `cpu_rasterizer` executable.

## Running the Rasterizer

After building the project, you can run the rasterizer with:

```bash
./cpu_rasterizer
```

You can pass command-line arguments to customize the rasterizer's behavior. For example:

```bash
./cpu_rasterizer --width 800 --height 600 --input model.obj --output output.png
```

## Project Structure

```
cpu-rasterizer/
├── CMakeLists.txt
├── src/
│   ├── application.hpp
│   ├── main.cpp
│   ├── gtkui/
│   │   ├── ui.cpp
│   │   └── ui.hpp
│   └── rasterizer/
│       ├── Framebuffer/
│       │   └── Framebuffer.hpp
│       ├── vector/
│       │   ├── triangle.h
│       │   ├── vec.c
│       │   └── vec.h
│       ├── colors.h
│       ├── cpu_rasterizer.hpp
│       ├── defs.h
│       └── ...
├── build/
├── test.ppm
└── README.md
```

## Contributing

Contributions are welcome! Please follow these guidelines:

1. **Fork the Repository**.
2. **Create a New Branch**: `git checkout -b feature/your-feature-name`.
3. **Commit Your Changes**: `git commit -m "Add some feature"`.
4. **Push to the Branch**: `git push origin feature/your-feature-name`.
5. **Create a Pull Request**.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For any inquiries or questions, please contact:

- **GitHub Issues**: [Open an Issue](https://github.com/Dogwalker-kryt/Cpu-rasterizer/issues)

---

Thank you for checking out the CPU Rasterizer project! Feel free to reach out if you have any questions or need 
further assistance.
