#pragma once

#include <stdint.h>
#include <memory.h>
#include <unistd.h>
#include <immintrin.h>
#include <cstdio>

namespace dwrast {

/**
 * @brief defines a pixel with x, y and color all stored as uint32_t
 */
struct pixel_t {
    uint32_t x;
    uint32_t y;
    uint32_t color;

    pixel_t(uint32_t X, uint32_t Y, uint32_t Color) : x(X), y(Y), color(Color) {}
};

/**
 * @brief Framebuffer class for storing pixel data in a 2D array.
 */
template<size_t Width, size_t Height>
class Framebuffer {
private:

    uint32_t pixels_[Width * Height] = {};

public:
    
    uint32_t *data() noexcept {
        return pixels_;
    }

    const uint32_t *data() const noexcept {
        return pixels_;
    }

    size_t height() {
        return Height;
    }

    size_t width() {
        return Width;
    }

    void clear(uint32_t color_) noexcept {
        
        static_assert((Width * Height) % 4 == 0, "Framebuffer size must be divisible by 4 for SIMD clear");
        
        const __m128i color_vec = _mm_set1_epi32(color_);
        size_t i{0};

        for (; i + 4 <= Width * Height; i += 4) {
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&pixels_[i]), color_vec);
        } 
    }

    pixel_t set_pixel(const size_t x_, const size_t y_, const uint32_t color_) noexcept {
        pixels_[y_ * Width + x_] = color_;
        return {x_, y_, color_};
    }

    uint32_t get_pixel_color(const size_t x_, const size_t y_) noexcept {
        return pixels_[y_ * Width + x_];
    }

    pixel_t get_pixel(const size_t x_, const size_t y_) const noexcept {
        return {x_, y_, pixels_[y_ * Width + x_]};
    }

    bool write_ppm(const char *file_name) {
        FILE *ppm_file = fopen(file_name, "wb");

        if (!ppm_file) return false;

        fprintf(ppm_file, "P6\n%d %d\n255\n", Width, Height);

        for (size_t y = 0; y < Height; ++y) {
            for (size_t x = 0; x < Width; ++x) {
                const uint32_t pixel = pixels_[y * Width + x];

                // Assumes pixel format: 0xAARRGGBB
                const unsigned char red   = (pixel >> 16) & 0xff;
                const unsigned char green = (pixel >> 8)  & 0xff;
                const unsigned char blue  = pixel & 0xff;

                fputc(red, ppm_file);
                fputc(green, ppm_file);
                fputc(blue, ppm_file);
            }
        }

        fclose(ppm_file);
        return true;
    }
};

typedef struct FB2 {
    uint32_t width;
    uint32_t heigth;
    uint32_t *buffer;

} FB2;

static inline FB2 *create_FB2(const size_t width, const size_t heigth) {
    FB2 *fb = new FB2();

    fb->heigth = heigth;
    fb->width = width;
    fb->buffer = new uint32_t[width * heigth];

    return fb;
}

__always_inline void destroy_FB2(FB2 *fb) {
    delete[] fb->buffer;
    delete fb;
}

__always_inline void clear_buf_FB2(FB2 *fb, uint32_t color_) noexcept {
        
    const __m128i color_vec = _mm_set1_epi32(color_);
    size_t i{0};

    for (; i + 4 <= fb->width * fb->heigth; i += 4) {
        _mm_storeu_si128(reinterpret_cast<__m128i*>(&fb->buffer[i]), color_vec);
    } 
}

static inline bool write_ppm_FB2(FB2 *fb, const char *file_name) {
    FILE *ppm_f = fopen(file_name, "wb");
    
    if (!ppm_f) return 0;
    
    fprintf(ppm_f, "P6\n%u %u\n255\n", fb->width, fb->heigth);

    for (size_t y = 0; y < fb->heigth; ++y) {
        for (size_t x = 0; x < fb->width; ++x) {
            const uint32_t pixel = fb->buffer[y * fb->width + x];

            // Assumes pixel format: 0xAARRGGBB
            const unsigned char red   = (pixel >> 16) & 0xff;
            const unsigned char green = (pixel >> 8)  & 0xff;
            const unsigned char blue  = pixel & 0xff;

            fputc(red, ppm_f);
            fputc(green, ppm_f);
            fputc(blue, ppm_f);
        }
    }

    fclose(ppm_f);
    return true;
}

__always_inline void set_pixel_FB2(FB2 *fb, const uint32_t x_, const uint32_t y_, const uint32_t color_) {
    fb->buffer[y_ * fb->width + x_] = color_;
}

__always_inline void set_pixel_pointer_FB2(uint32_t *pixel, const uint32_t color) {
    *pixel = color;
}

__always_inline uint32_t get_pixel_color_FB2(FB2 *fb, const uint32_t x_, const uint32_t y_) {
    return fb->buffer[y_ * fb->width + x_];
}

__always_inline pixel_t get_pixel_FB2(FB2 *fb, const uint32_t x_, const uint32_t y_) {
    return {x_, y_, fb->buffer[y_ * fb->width + x_]};
}

};