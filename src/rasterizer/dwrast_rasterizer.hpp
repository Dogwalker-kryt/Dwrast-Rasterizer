#pragma once

#include "Framebuffer/Framebuffer.hpp"
#include "vector/vec.h"
#include "vector/triangle.h"
#include "colors.h"
#include "defs.h"
#include "boundingbox/boundingbox.h"
#include "math/absolute.h"
#include <algorithm>
#include <climits>

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
        int32_t x_distance = absolute_f(P1.x - P0.x);
        int32_t y_distance = absolute_f(P1.y - P0.y);
        int8_t x_direction = (P0.x < P1.x) ? 1 : -1;
        int8_t y_direction = (P0.y < P1.y) ? 1 : -1;
        int32_t err = x_distance - y_distance;

        while (true) {
            if (P0.x >= 0 && P0.y >= 0 &&
                static_cast<uint32_t>(P0.x) < fb->width &&
                static_cast<uint32_t>(P0.y) < fb->heigth) {
                const size_t index = static_cast<size_t>(P0.y) * fb->width + static_cast<size_t>(P0.x);
                set_pixel_pointer_FB2(&fb->buffer[index], color);
            }

            if (P0.x == P1.x && P0.y == P1.y) break;

            int e2 = 2 * err;
            if (e2 > -y_distance) {
                err -= y_distance;
                P0.x += x_direction;
            }
            if (e2 < x_distance) {
                err += x_distance;
                P0.y += y_direction;
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

    static inline void fill_triangle_avx2(dwrast::FB2 *fb, triangle_t *triangle, const color_t color) {
        bounding_box_t bbox = calculate_boundingbox(triangle);
        if (!clip_triangle_bbox(fb, &bbox)) return;

        const __m256i color_vec = _mm256_set1_epi32(color);
        int32_t x = bbox.min_x;
        for (int32_t y = bbox.min_y; y <= bbox.max_y; ++y) {
            x = bbox.min_x;
            for (; bbox.max_x - x >= 7; x += 8) {
                bool all_inside = true;
                for (int32_t lane = 0; lane < 8; ++lane) {
                    if (!triangle_contains_pixel(triangle, {x + lane, y})) {
                        all_inside = false;
                        break;
                    }
                }

                uint32_t *pixels = &fb->buffer[static_cast<size_t>(y) * fb->width + static_cast<uint32_t>(x)];
                if (all_inside) {
                    _mm256_storeu_si256(reinterpret_cast<__m256i*>(pixels), color_vec);
                } else {
                    for (int32_t lane = 0; lane < 8; ++lane) {
                        if (triangle_contains_pixel(triangle, {x + lane, y})) {
                            pixels[lane] = color;
                        }
                    }
                }
            }

            for (; x <= bbox.max_x; ++x) {
                if (triangle_contains_pixel(triangle, {x, y})) {
                    const size_t index = static_cast<size_t>(y) * fb->width + static_cast<uint32_t>(x);
                    set_pixel_pointer_FB2(&fb->buffer[index], color);
                }
            }
        }
    }

    static inline void draw_triangle(dwrast::FB2 *fb, triangle_t *triangle, color_t fill_color, color_t outline_color) {
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