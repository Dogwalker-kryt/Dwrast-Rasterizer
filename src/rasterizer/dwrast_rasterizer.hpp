#pragma once

#include "Framebuffer/Framebuffer.hpp"
#include "vector/vec.h"
#include "vector/triangle.h"
#include "colors.h"
#include "defs.h"

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
};