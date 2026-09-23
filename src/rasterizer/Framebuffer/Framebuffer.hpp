#pragma once

#include <stdint.h>
#include <memory.h>
#include <unistd.h>
#include <immintrin.h>

namespace dwrast {

/**
 * @brief defines a pixel with x, y and color all stored as uint32_t
 */
struct pixel_t {
    size_t x;
    size_t y;
    uint32_t color;

    pixel_t(size_t X, size_t Y, uint32_t Color) : x(X), y(Y), color(Color) {}
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
};



};