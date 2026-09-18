#pragma once

#include <stdint.h>
#include <memory.h>
#include <unistd.h>
#include <immintrin.h>

namespace cpu_rast {

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
template<size_t width, size_t height>
class Framebuffer {
private:

    uint32_t pixels_[width * height] = {};

public:
    
    uint32_t *data() noexcept {
        return pixels_;
    }

    const uint32_t *data() const noexcept {
        return pixels_;
    }

    void clear(uint32_t color_) noexcept {
        
        static_assert((width * height) % 4 == 0, "Framebuffer size must be divisible by 4 for SIMD clear");
        
        const __m128i color_vec = _mm_set1_epi32(color_);
        size_t i{0};

        for (; i + 4 <= width * height; i += 4) {
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&pixels_[i]), color_vec);
        } 
    }

    pixel_t set_pixel(const size_t x_, const size_t y_, const uint32_t color_) {
        uint32_t x = static_cast<uint32_t>(x_);
        uint32_t y = static_cast<uint32_t>(y_);

        pixel_t pixel{x, y, color_};
        pixels_[y_ * width + x_] = color_;

        return pixel;
    }

    pixel_t get_pixel(const size_t x_, const size_t y_) const {
        uint32_t x = static_cast<uint32_t>(x_);
        uint32_t y = static_cast<uint32_t>(y_);

        pixel_t pixel = {x, y, pixels_[x * width + x]};
        
        return pixel;
    }
};



};