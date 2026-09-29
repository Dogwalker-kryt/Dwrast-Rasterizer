#pragma once

#include "Framebuffer/Framebuffer.hpp"
#include "vector/vec.h"
#include "vector/triangle.h"
#include "colors.h"
#include "defs.h"
#include "boundingbox/boundingbox.h"
#include "math/absolute.h"

namespace dwrast {

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
            if (P0.x >= 0 && P0.y >= 0 && static_cast<size_t>(P0.x) < fb->width && static_cast<size_t>(P0.y) < fb->heigth) {
                fb->buffer[P0.y * fb->width + P0.x] = color;
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
        const bounding_box_t bbox = calculate_boundingbox(triangle);

        for (int32_t y = bbox.min_y; y <= bbox.max_y; ++y) {
            for (int32_t x = bbox.min_x; x <= bbox.max_x; ++x) {
                if (triangle_contains_pixel(triangle, x, y)) {
                    set_pixel_FB2(fb, x, y, color);
                }
            }
        }
    }

    static inline void draw_triangle(dwrast::FB2 *fb, triangle_t *triangle, color_t fill_color, color_t outline_color) {
        fill_triangle(fb, triangle, fill_color);
        
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