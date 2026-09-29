#pragma once

#include "vec.h"
#include <stdbool.h>

typedef struct vertex_t {
    vec4f_t pos;
    color_t color;
} vertex_t;

typedef struct triangle_t {
    vertex_t v0, v1, v2;
} triangle_t; 

typedef struct vec2_triangle_t {
    vec2f_t v0_to_v1;
    vec2f_t v1_to_v2;
    vec2f_t v2_to_v0;
} vec2_triangle_t;

typedef struct tri_raster_t {
    float x0, y0, z0, w0;
    float x1, y1, z1, w1;
    float x2, y2, z2, w2;
    float x3, y3, z3, w3;

    color_t c1, c2, c3;
} tri_raster_t;

typedef struct vertex_arr_t {
    vec4f_t *pos;
    color_t *color;
} vertex_arr_t;



/**
 * @brief color is always color of a 
 */
static inline vertex_t vertex_add(const vertex_t a, const vertex_t b) {
    return VEC_INIT(vertex_t,
        vec4f_add(a.pos, b.pos),
        a.color
    );
}

/**
 * @brief color is always color of a 
 */
static inline vertex_t vertex_add3(const vertex_t a, const vertex_t b, const vertex_t c) {
    return VEC_INIT(vertex_t,
        vec4f_add3(a.pos, b.pos, c.pos),
        a.color
    );
}

/**
 * @brief color is always color of a 
 */
static inline vertex_t vertex_sub(const vertex_t a, const vertex_t b) {
    return VEC_INIT(vertex_t,
        vec4f_sub(a.pos, b.pos),
        a.color
    );
}

/**
 * @brief color is always color of a 
 */
static inline vertex_t vertex_sub3(const vertex_t a, const vertex_t b, const vertex_t c) {
    return VEC_INIT(vertex_t,
        vec4f_sub3(a.pos, b.pos, c.pos),
        a.color
    );
}

/**
 * @brief color is always color of a 
 */
static inline vertex_t vertex_mul(const vertex_t a, const vertex_t b) {
    return VEC_INIT(vertex_t,
        vec4f_mul(a.pos, b.pos),
        a.color
    );
}

/**
 * @brief color is always color of a 
 */
static inline vertex_t vertex_mul3(const vertex_t a, const vertex_t b, const vertex_t c) {
    return VEC_INIT(vertex_t,
        vec4f_mul3(a.pos, b.pos, c.pos),
        a.color
    );
}

/**
 * @brief color is always color of a 
 */
static inline vertex_t vertex_div(const vertex_t a, const vertex_t b) {
    return VEC_INIT(vertex_t,
        vec4f_div(a.pos, b.pos),
        a.color
    );
}

/**
 * @brief color is always color of a 
 */
static inline vertex_t vertex_div3(const vertex_t a, const vertex_t b, const vertex_t c) {
    return VEC_INIT(vertex_t,
        vec4f_div3(a.pos, b.pos, c.pos),
        a.color
    );
}


static inline vertex_t vertex_scale(const vertex_t v, float s) {
    return VEC_INIT(vertex_t,
        vec4f_scale(v.pos, s),
        v.color
    );
}

/**
 * @brief color is always color of a 
 */
static inline vertex_t vertex_lerp(const vertex_t a, const vertex_t b, float t) {
    return VEC_INIT(vertex_t, 
        vec4f_lerp(a.pos, b.pos, t),
        a.color
    );
}

__always_inline triangle_t create_triangle_from_vertex(const vertex_t v0, const vertex_t v1, const vertex_t v2) {
    return VEC_INIT(triangle_t,
        v0, v1, v2
    );
}

static inline bool triangle_contains_pixel(triangle_t *triangle, const int32_t x, const int32_t y) {
    vec2f_t a = { triangle->v0.pos.x, triangle->v0.pos.y };
    vec2f_t b = { triangle->v1.pos.x, triangle->v1.pos.y };
    vec2f_t c = { triangle->v2.pos.x, triangle->v2.pos.y };

    const float e1 = vec2f_edge(a, b, VEC_INIT(rpixel_t, x, y));
    const float e2 = vec2f_edge(b, c, VEC_INIT(rpixel_t, x, y));
    const float e3 = vec2f_edge(c, a, VEC_INIT(rpixel_t, x, y));
    const float area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);

    if (area >= 0.0f) {
        return e1 >= 0.0f && e2 >= 0.0f && e3 >= 0.0f;
    }

    return e1 <= 0.0f && e2 <= 0.0f && e3 <= 0.0f;
}