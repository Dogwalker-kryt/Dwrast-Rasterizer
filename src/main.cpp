#include <iostream>
#include "rasterizer/cpu_rasterizer.hpp"
#include "rasterizer/colors.h"

using namespace cpu_rast;

int main(int argc, char** argv) {

    printf("Starting CPU Rasterizer...\n");

    printf("Framebuffer test...\n");
    printf("Framebuffer size: 700x800\n");

    Framebuffer<700, 800> frame_buf;
    
    frame_buf.set_pixel(50, 30, WHITE);

    frame_buf.clear(BLACK);

    pixel_t pixel2 = frame_buf.get_pixel(50, 30);

    if (pixel2.color == WHITE) {
        printf("%s[ERROR]%s SIMD didnt work\n", RED_ANSI, RESET_ANSI);
        return 1;
    }

    printf("%sSIMD did work%s\n", GREEN_ANSI, RESET_ANSI);

    return 0;
}