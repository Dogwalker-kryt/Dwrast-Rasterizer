#pragma once

#include "../Framebuffer/Framebuffer.hpp"
#include "../vector/triangle.h"
#include "../defs.h"
#include "../math/min_max.h"

typedef struct bounding_box_t {
    int32_t max_x = 0;
    int32_t min_x = 0;
    int32_t max_y = 0;
    int32_t min_y = 0;
} bounding_box_t;

static inline bounding_box_t calculate_boundingbox(const triangle_t *triangle) {
    return {
        static_cast<int32_t>(max_f_inline(triangle->v0.pos.x, max_f_inline(triangle->v1.pos.x, triangle->v2.pos.x))),
        static_cast<int32_t>(min_f_inline(triangle->v0.pos.x, min_f_inline(triangle->v1.pos.x, triangle->v2.pos.x))),
        static_cast<int32_t>(max_f_inline(triangle->v0.pos.y, max_f_inline(triangle->v1.pos.y, triangle->v2.pos.y))),
        static_cast<int32_t>(min_f_inline(triangle->v0.pos.y, min_f_inline(triangle->v1.pos.y, triangle->v2.pos.y)))
    };
}