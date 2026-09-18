#include <iostream>
#include "rasterizer/cpu_rasterizer.hpp"

using namespace cpu_rast;

int main(int argc, char** argv) {

    
    Framebuffer<700, 800> frame_buf;
    
    frame_buf.set_pixel(50, 30, 0xFFFFFFFF);

    frame_buf.clear(0xFF000000);

    pixel_t pixel2 = frame_buf.get_pixel(50, 30);

    if (pixel2.color == 4294967295U) {
        printf("SIMD didnt work\n");
        return 1;
    }

    printf("SIMD did work\n");

    return 0;
}