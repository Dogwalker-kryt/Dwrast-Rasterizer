#pragma once

#include "Framebuffer/Framebuffer.hpp"
#include "vector/vec.h"
#include "vector/triangle.h"
#include "colors.h"
#include "defs.h"
#include "boundingbox/boundingbox.h"
#include "math/absolute.h"
#include "simd/triangle_avx2.hpp"
#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdlib>

namespace dwrast {

    static inline bool clip_triangle_bbox(const dwrast::FB2 *fb, bounding_box_t *bbox) {
        if (!fb || !fb->buffer || fb->width == 0 || fb->heigth == 0) return false;

        const int32_t max_x = static_cast<int32_t>(std::min<uint32_t>(fb->width - 1, INT32_MAX - 1));
        const int32_t max_y = static_cast<int32_t>(std::min<uint32_t>(fb->heigth - 1, INT32_MAX - 1));
        bbox->min_x = std::max<int32_t>(bbox->min_x, 0);
        bbox->min_y = std::max<int32_t>(bbox->min_y, 0);
        bbox->max_x = std::min<int32_t>(bbox->max_x, max_x);
        bbox->max_y = std::min<int32_t>(bbox->max_y, max_y);
        return bbox->min_x <= bbox->max_x && bbox->min_y <= bbox->max_y;
    }

    /**
     * @brief computes the 2D screen coordinates of a vertex after perspective division and viewport transformation.
     * @param v The vertex to project.
     * @param viewport_width The width of the viewport.
     * @param viewport_height The height of the viewport.
     * @return The 2D screen coordinates of the vertex.
     */
    static inline vec2f_t project_vertex(vertex_t *v, int viewport_width, int viewport_height) {
        float inv_w = 1.0f / v->pos.w;
        return VEC_INIT(vec2f_t,
            (v->pos.x * inv_w + 1.0f) * viewport_width / 2.0f,
            (1.0f - v->pos.y * inv_w) * viewport_height / 2.0f
        );
    }

    /**
     * @brief Computes the signed area of a triangle in 2D space.
     * @param t The triangle for which to compute the area.
     * @return The signed area of the triangle.
     */
    static inline float triangle_area2(const triangle_t *t) {
        const float ax = t->v1.pos.x - t->v0.pos.x;
        const float ay = t->v1.pos.y - t->v0.pos.y;

        const float bx = t->v2.pos.x - t->v0.pos.x;
        const float by = t->v2.pos.y - t->v0.pos.y;

        return ax * by - ay * bx;
    }

    /**
     * @brief Checks if a triangle is degenerate (i.e., has zero area).
     * @param tri The triangle to check.
     * @return true if the triangle is degenerate, false otherwise.
     */
    static inline bool triangle_is_degen(const triangle_t *tri) {
        return triangle_area2(tri) == 0.0f;
    }

    /**
     * @brief draws line between 2 points to fb
     */
    static inline void draw_line(dwrast::FB2 *fb, rpixel_t P0, rpixel_t P1, color_t color) {
        if (!fb || !fb->buffer) return;

        int64_t x = P0.x;
        int64_t y = P0.y;
        const int64_t x1 = P1.x;
        const int64_t y1 = P1.y;
        const int64_t dx = std::abs(x1 - x);
        const int64_t dy = std::abs(y1 - y);
        const int64_t sx = (x < x1) ? 1 : -1;
        const int64_t sy = (y < y1) ? 1 : -1;
        int64_t error = dx - dy;

        while (true) {
            if (x >= 0 && y >= 0 && static_cast<uint64_t>(x) < fb->width &&
                static_cast<uint64_t>(y) < fb->heigth) {
                const size_t index = static_cast<size_t>(y) * fb->width + static_cast<size_t>(x);
                set_pixel_pointer_FB2(&fb->buffer[index], color);
            }

            if (x == x1 && y == y1) break;

            const int64_t twice_error = 2 * error;
            if (twice_error > -dy) {
                error -= dy;
                x += sx;
            }
            if (twice_error < dx) {
                error += dx;
                y += sy;
            }
        }
    }

    /**
     * @brief fills all pixels in triangle with color
     * @param fb framebuffer
     * @param triangle is not checkd internaly, must be checked externaly
     * @param color the color
     */
    static inline void fill_triangle(dwrast::FB2 *fb, triangle_t *triangle, const color_t color) {
        if (!fb || !triangle || triangle_is_degen(triangle)) return;
        bounding_box_t bbox = calculate_boundingbox(triangle);
        if (!clip_triangle_bbox(fb, &bbox)) return;

        for (int32_t y = bbox.min_y; y <= bbox.max_y; ++y) {
            for (int32_t x = bbox.min_x; x <= bbox.max_x; ++x) {
                if (triangle_contains_pixel(triangle, {x, y})) {
                    set_pixel_FB2(fb, static_cast<uint32_t>(x), static_cast<uint32_t>(y), color);
                }
            }
        }
    }

    /**
     * @brief fills all pixels in triangle with color using AVX2
     * @param fb framebuffer
     * @param triangle is not checkd internaly, must be checked externaly
     * @param color the color
     * @warning every parameter must be validated by the caller, the function it self doesnt check them
     */
    static inline void fill_triangle_avx2(dwrast::FB2 *fb, triangle_t *triangle, const color_t color) {
        bounding_box_t bbox = calculate_boundingbox(triangle);
        if (!clip_triangle_bbox(fb, &bbox)) return;

        simd::fill_triangle_8x8(fb, triangle, bbox, color);
    }

    static inline void draw_triangle(dwrast::FB2 *fb, triangle_t *triangle, color_t fill_color, color_t outline_color) {
        if (!fb || !fb->buffer || !triangle) return;
        fill_triangle_avx2(fb, triangle, fill_color);
        
        rpixel_t P0 = {
            static_cast<int32_t>(triangle->v0.pos.x),
            static_cast<int32_t>(triangle->v0.pos.y)
        };

        rpixel_t P1 = {
            static_cast<int32_t>(triangle->v1.pos.x),
            static_cast<int32_t>(triangle->v1.pos.y)
        };

        rpixel_t P2 = {
            static_cast<int32_t>(triangle->v2.pos.x),
            static_cast<int32_t>(triangle->v2.pos.y)
        };

        draw_line(fb, P0, P1, outline_color);
        draw_line(fb, P1, P2, outline_color);
        draw_line(fb, P2, P0, outline_color);
    }

};