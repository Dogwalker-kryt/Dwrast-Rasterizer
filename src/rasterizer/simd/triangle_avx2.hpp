#pragma once

#include "../Framebuffer/Framebuffer.hpp"
#include "../vector/triangle.h"
#include "../boundingbox/boundingbox.h"
#include <immintrin.h>
#include <cstddef>
#include <cstdint>

namespace dwrast::simd {

struct edge_plane_t {
    float anchor_x;
    float anchor_y;
    float dx;
    float dy;
    float x_step;
    float y_step;
};

__always_inline edge_plane_t make_edge_plane(const vec2f_t a, const vec2f_t b) {
    const float dx = b.x - a.x;
    const float dy = b.y - a.y;
    return {a.x, a.y, dx, dy, -dy, dx};
}

__always_inline float edge_at(const edge_plane_t edge, const int32_t x, const int32_t y) {
    return edge.dx * (static_cast<float>(y) - edge.anchor_y) - edge.dy * (static_cast<float>(x) - edge.anchor_x);
}

__always_inline __m256 edge_lanes(const __m256 edge_at_row_start, const __m256 x_step_lanes) {
    return _mm256_add_ps(edge_at_row_start, x_step_lanes);
}

__always_inline __m256i triangle_coverage_mask(const __m256 e0, const __m256 e1, const __m256 e2, const __m256 zero, const __m256i valid_lanes, const bool positive_winding) {
    __m256 m0;
    __m256 m1;
    __m256 m2;
    if (positive_winding) {
        m0 = _mm256_cmp_ps(e0, zero, _CMP_GE_OQ);
        m1 = _mm256_cmp_ps(e1, zero, _CMP_GE_OQ);
        m2 = _mm256_cmp_ps(e2, zero, _CMP_GE_OQ);
    } else {
        m0 = _mm256_cmp_ps(e0, zero, _CMP_LE_OQ);
        m1 = _mm256_cmp_ps(e1, zero, _CMP_LE_OQ);
        m2 = _mm256_cmp_ps(e2, zero, _CMP_LE_OQ);
    }

    return _mm256_and_si256(
        _mm256_and_si256(_mm256_castps_si256(m0), _mm256_castps_si256(m1)),
        _mm256_and_si256(_mm256_castps_si256(m2), valid_lanes)
    );
}

__always_inline void masked_store_row(uint32_t *destination, const __m256i coverage, const __m256i color) {
    _mm256_maskstore_epi32(reinterpret_cast<int*>(destination), coverage, color);
}

__always_inline void fill_triangle_8x8(FB2 *fb, const triangle_t *triangle, const bounding_box_t bbox, const color_t color) {
    const vec2f_t a{triangle->v0.pos.x, triangle->v0.pos.y};
    const vec2f_t b{triangle->v1.pos.x, triangle->v1.pos.y};
    const vec2f_t c{triangle->v2.pos.x, triangle->v2.pos.y};

    const float area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    if (area == 0.0f) return;

    const edge_plane_t edges[3] = {
        make_edge_plane(a, b),
        make_edge_plane(b, c),
        make_edge_plane(c, a)
    };

    const __m256 offsets = _mm256_setr_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);

    const __m256 x_steps[3] = {
        _mm256_mul_ps(offsets, _mm256_set1_ps(edges[0].x_step)),
        _mm256_mul_ps(offsets, _mm256_set1_ps(edges[1].x_step)),
        _mm256_mul_ps(offsets, _mm256_set1_ps(edges[2].x_step))
    };

    const __m256 zero = _mm256_setzero_ps();
    const __m256i color_vec = _mm256_set1_epi32(static_cast<int>(color));

    const bool positive_winding = area > 0.0f;

    for (int64_t tile_y = bbox.min_y; tile_y <= bbox.max_y; tile_y += 8) {
        const int32_t first_y = static_cast<int32_t>(tile_y);

        for (int64_t tile_x = bbox.min_x; tile_x <= bbox.max_x; tile_x += 8) {
            const int32_t first_x = static_cast<int32_t>(tile_x);
            const int32_t remaining_x = bbox.max_x - first_x + 1;
            const int32_t lane_count = remaining_x < 8 ? remaining_x : 8;
            const __m256i valid_lanes = _mm256_setr_epi32(
                lane_count > 0 ? -1 : 0, lane_count > 1 ? -1 : 0,
                lane_count > 2 ? -1 : 0, lane_count > 3 ? -1 : 0,
                lane_count > 4 ? -1 : 0, lane_count > 5 ? -1 : 0,
                lane_count > 6 ? -1 : 0, lane_count > 7 ? -1 : 0
            );

            const float edge_starts[3] = {
                edge_at(edges[0], first_x, first_y),
                edge_at(edges[1], first_x, first_y),
                edge_at(edges[2], first_x, first_y)
            };

            __m256 row_starts[3] = {
                _mm256_set1_ps(edge_starts[0]),
                _mm256_set1_ps(edge_starts[1]),
                _mm256_set1_ps(edge_starts[2])
            };

            const __m256 row_steps[3] = {
                _mm256_set1_ps(edges[0].y_step),
                _mm256_set1_ps(edges[1].y_step),
                _mm256_set1_ps(edges[2].y_step)
            };

            for (int32_t row = 0; row < 8 && tile_y + row <= bbox.max_y; ++row) {
                const __m256 e0 = edge_lanes(row_starts[0], x_steps[0]);
                const __m256 e1 = edge_lanes(row_starts[1], x_steps[1]);
                const __m256 e2 = edge_lanes(row_starts[2], x_steps[2]);

                const __m256i coverage = triangle_coverage_mask(e0, e1, e2, zero, valid_lanes, positive_winding);
                const size_t index = static_cast<size_t>(tile_y + row) * fb->width + static_cast<size_t>(first_x);

                masked_store_row(&fb->buffer[index], coverage, color_vec);

                row_starts[0] = _mm256_add_ps(row_starts[0], row_steps[0]);
                row_starts[1] = _mm256_add_ps(row_starts[1], row_steps[1]);
                row_starts[2] = _mm256_add_ps(row_starts[2], row_steps[2]);
            }
        }
    }
}

} // namespace dwrast::simd