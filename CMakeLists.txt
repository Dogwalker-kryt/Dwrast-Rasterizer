cmake_minimum_required(VERSION 3.20)

project(cpu_rasterizer
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

add_executable(cpu_rasterizer
    src/main.cpp
    src/rasterizer/vector/vec.c
    src/gtkui/ui.cpp
)

target_link_libraries(cpu_rasterizer
    PRIVATE
        PkgConfig::GTK4
)

target_compile_options(cpu_rasterizer
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -O3
        -march=native
        -flto
)
