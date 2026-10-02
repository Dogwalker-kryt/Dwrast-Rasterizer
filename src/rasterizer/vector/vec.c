#include "vec.h"

static inline float hsum8_ps(const __m256 values) {
    const __m256 pairs = _mm256_hadd_ps(values, values);
    const __m256 quads = _mm256_hadd_ps(pairs, pairs);
    const __m256 total = _mm256_add_ps(quads, _mm256_permute2f128_ps(quads, quads, 0x01));
    return _mm_cvtss_f32(_mm256_castps256_ps128(total));
}

static inline float hprod8_ps(const __m256 values) {
    const __m256 pairs = _mm256_mul_ps(values, _mm256_permute_ps(values, _MM_SHUFFLE(2, 3, 0, 1)));
    const __m256 quads = _mm256_mul_ps(pairs, _mm256_permute_ps(pairs, _MM_SHUFFLE(1, 0, 3, 2)));
    const __m256 total = _mm256_mul_ps(quads, _mm256_permute2f128_ps(quads, quads, 0x01));
    return _mm_cvtss_f32(_mm256_castps256_ps128(total));
}

inline vec2f_t from_vec2_arr(const vec2f_arr_t *arr, const size_t n) {
    return (vec2f_t){arr->x[n], arr->y[n]};
}

inline vec3f_t from_vec3_arr(const vec3f_arr_t *arr, size_t n) {
    return (vec3f_t){arr->x[n], arr->y[n], arr->z[n]};
}

inline vec4f_t from_vec4_arr(const vec4f_arr_t *arr, size_t n) {
    return (vec4f_t){arr->x[n], arr->y[n], arr->z[n], arr->w[n]};
}

vec2f_t *vec2f_arr_to_aos(const vec2f_arr_t *restrict arr, vec2f_t *restrict out, size_t count) {
    for (size_t i = 0; i < count; ++i) out[i] = (vec2f_t){arr->x[i], arr->y[i]};
    return out;
}

vec3f_t *vec3f_arr_to_aos(const vec3f_arr_t *restrict arr, vec3f_t *restrict out, size_t count) {
    for (size_t i = 0; i < count; ++i) out[i] = (vec3f_t){arr->x[i], arr->y[i], arr->z[i]};
    return out;
}

vec4f_t *vec4f_arr_to_aos(const vec4f_arr_t *restrict arr, vec4f_t *restrict out, size_t count) {
    for (size_t i = 0; i < count; ++i) out[i] = (vec4f_t){arr->x[i], arr->y[i], arr->z[i], arr->w[i]};
    return out;
}

#define DEFINE_REDUCTIONS_2D(PREFIX, TYPE, ARRAY, X, Y) \
TYPE PREFIX##_n_add(const ARRAY *restrict values, size_t count) { \
    __m256 sx = _mm256_setzero_ps(), sy = _mm256_setzero_ps(); \
    size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        sx = _mm256_add_ps(sx, _mm256_loadu_ps(values->X + i)); \
        sy = _mm256_add_ps(sy, _mm256_loadu_ps(values->Y + i)); \
    } \
    float x = hsum8_ps(sx), y = hsum8_ps(sy); \
    for (; i < count; ++i) { x += values->X[i]; y += values->Y[i]; } \
    return (TYPE){x, y}; \
} \
TYPE PREFIX##_n_sub(const ARRAY *restrict values, size_t count) { \
    if (count == 0) return (TYPE){0}; \
    const float x0 = values->X[0], y0 = values->Y[0]; \
    __m256 sx = _mm256_setzero_ps(), sy = _mm256_setzero_ps(); \
    size_t i = 1; \
    for (; i + 8 <= count; i += 8) { \
        sx = _mm256_add_ps(sx, _mm256_loadu_ps(values->X + i)); \
        sy = _mm256_add_ps(sy, _mm256_loadu_ps(values->Y + i)); \
    } \
    float x = hsum8_ps(sx), y = hsum8_ps(sy); \
    for (; i < count; ++i) { x += values->X[i]; y += values->Y[i]; } \
    return (TYPE){x0 - x, y0 - y}; \
} \
TYPE PREFIX##_n_mul(const ARRAY *restrict values, size_t count) { \
    __m256 px = _mm256_set1_ps(1.0f), py = _mm256_set1_ps(1.0f); \
    size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        px = _mm256_mul_ps(px, _mm256_loadu_ps(values->X + i)); \
        py = _mm256_mul_ps(py, _mm256_loadu_ps(values->Y + i)); \
    } \
    float x = hprod8_ps(px), y = hprod8_ps(py); \
    for (; i < count; ++i) { x *= values->X[i]; y *= values->Y[i]; } \
    return (TYPE){x, y}; \
} \
TYPE PREFIX##_n_div(const ARRAY *restrict values, size_t count) { \
    if (count == 0) return (TYPE){0}; \
    const float x0 = values->X[0], y0 = values->Y[0]; \
    __m256 px = _mm256_set1_ps(1.0f), py = _mm256_set1_ps(1.0f); \
    size_t i = 1; \
    for (; i + 8 <= count; i += 8) { \
        px = _mm256_mul_ps(px, _mm256_loadu_ps(values->X + i)); \
        py = _mm256_mul_ps(py, _mm256_loadu_ps(values->Y + i)); \
    } \
    float x = hprod8_ps(px), y = hprod8_ps(py); \
    for (; i < count; ++i) { x *= values->X[i]; y *= values->Y[i]; } \
    return (TYPE){x0 / x, y0 / y}; \
}

#define DEFINE_REDUCTIONS_3D(PREFIX, TYPE, ARRAY, X, Y, Z) \
TYPE PREFIX##_n_add(const ARRAY *restrict values, size_t count) { \
    __m256 sx = _mm256_setzero_ps(), sy = _mm256_setzero_ps(), sz = _mm256_setzero_ps(); \
    size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        sx = _mm256_add_ps(sx, _mm256_loadu_ps(values->X + i)); \
        sy = _mm256_add_ps(sy, _mm256_loadu_ps(values->Y + i)); \
        sz = _mm256_add_ps(sz, _mm256_loadu_ps(values->Z + i)); \
    } \
    float x = hsum8_ps(sx), y = hsum8_ps(sy), z = hsum8_ps(sz); \
    for (; i < count; ++i) { x += values->X[i]; y += values->Y[i]; z += values->Z[i]; } \
    return (TYPE){x, y, z}; \
} \
TYPE PREFIX##_n_sub(const ARRAY *restrict values, size_t count) { \
    if (count == 0) return (TYPE){0}; \
    const float x0 = values->X[0], y0 = values->Y[0], z0 = values->Z[0]; \
    __m256 sx = _mm256_setzero_ps(), sy = _mm256_setzero_ps(), sz = _mm256_setzero_ps(); \
    size_t i = 1; \
    for (; i + 8 <= count; i += 8) { \
        sx = _mm256_add_ps(sx, _mm256_loadu_ps(values->X + i)); \
        sy = _mm256_add_ps(sy, _mm256_loadu_ps(values->Y + i)); \
        sz = _mm256_add_ps(sz, _mm256_loadu_ps(values->Z + i)); \
    } \
    float x = hsum8_ps(sx), y = hsum8_ps(sy), z = hsum8_ps(sz); \
    for (; i < count; ++i) { x += values->X[i]; y += values->Y[i]; z += values->Z[i]; } \
    return (TYPE){x0 - x, y0 - y, z0 - z}; \
} \
TYPE PREFIX##_n_mul(const ARRAY *restrict values, size_t count) { \
    __m256 px = _mm256_set1_ps(1.0f), py = _mm256_set1_ps(1.0f), pz = _mm256_set1_ps(1.0f); \
    size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        px = _mm256_mul_ps(px, _mm256_loadu_ps(values->X + i)); \
        py = _mm256_mul_ps(py, _mm256_loadu_ps(values->Y + i)); \
        pz = _mm256_mul_ps(pz, _mm256_loadu_ps(values->Z + i)); \
    } \
    float x = hprod8_ps(px), y = hprod8_ps(py), z = hprod8_ps(pz); \
    for (; i < count; ++i) { x *= values->X[i]; y *= values->Y[i]; z *= values->Z[i]; } \
    return (TYPE){x, y, z}; \
} \
TYPE PREFIX##_n_div(const ARRAY *restrict values, size_t count) { \
    if (count == 0) return (TYPE){0}; \
    const float x0 = values->X[0], y0 = values->Y[0], z0 = values->Z[0]; \
    __m256 px = _mm256_set1_ps(1.0f), py = _mm256_set1_ps(1.0f), pz = _mm256_set1_ps(1.0f); \
    size_t i = 1; \
    for (; i + 8 <= count; i += 8) { \
        px = _mm256_mul_ps(px, _mm256_loadu_ps(values->X + i)); \
        py = _mm256_mul_ps(py, _mm256_loadu_ps(values->Y + i)); \
        pz = _mm256_mul_ps(pz, _mm256_loadu_ps(values->Z + i)); \
    } \
    float x = hprod8_ps(px), y = hprod8_ps(py), z = hprod8_ps(pz); \
    for (; i < count; ++i) { x *= values->X[i]; y *= values->Y[i]; z *= values->Z[i]; } \
    return (TYPE){x0 / x, y0 / y, z0 / z}; \
}

#define DEFINE_REDUCTIONS_4D(PREFIX, TYPE, ARRAY, X, Y, Z, W) \
TYPE PREFIX##_n_add(const ARRAY *values, size_t count) { \
    __m256 sx = _mm256_setzero_ps(), sy = _mm256_setzero_ps(), sz = _mm256_setzero_ps(), sw = _mm256_setzero_ps(); \
    size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        sx = _mm256_add_ps(sx, _mm256_loadu_ps(values->X + i)); \
        sy = _mm256_add_ps(sy, _mm256_loadu_ps(values->Y + i)); \
        sz = _mm256_add_ps(sz, _mm256_loadu_ps(values->Z + i)); \
        sw = _mm256_add_ps(sw, _mm256_loadu_ps(values->W + i)); \
    } \
    float x = hsum8_ps(sx), y = hsum8_ps(sy), z = hsum8_ps(sz), w = hsum8_ps(sw); \
    for (; i < count; ++i) { x += values->X[i]; y += values->Y[i]; z += values->Z[i]; w += values->W[i]; } \
    return (TYPE){x, y, z, w}; \
} \
TYPE PREFIX##_n_sub(const ARRAY *values, size_t count) { \
    if (count == 0) return (TYPE){0}; \
    const float x0 = values->X[0], y0 = values->Y[0], z0 = values->Z[0], w0 = values->W[0]; \
    __m256 sx = _mm256_setzero_ps(), sy = _mm256_setzero_ps(), sz = _mm256_setzero_ps(), sw = _mm256_setzero_ps(); \
    size_t i = 1; \
    for (; i + 8 <= count; i += 8) { \
        sx = _mm256_add_ps(sx, _mm256_loadu_ps(values->X + i)); \
        sy = _mm256_add_ps(sy, _mm256_loadu_ps(values->Y + i)); \
        sz = _mm256_add_ps(sz, _mm256_loadu_ps(values->Z + i)); \
        sw = _mm256_add_ps(sw, _mm256_loadu_ps(values->W + i)); \
    } \
    float x = hsum8_ps(sx), y = hsum8_ps(sy), z = hsum8_ps(sz), w = hsum8_ps(sw); \
    for (; i < count; ++i) { x += values->X[i]; y += values->Y[i]; z += values->Z[i]; w += values->W[i]; } \
    return (TYPE){x0 - x, y0 - y, z0 - z, w0 - w}; \
} \
TYPE PREFIX##_n_mul(const ARRAY *values, size_t count) { \
    __m256 px = _mm256_set1_ps(1.0f), py = _mm256_set1_ps(1.0f), pz = _mm256_set1_ps(1.0f), pw = _mm256_set1_ps(1.0f); \
    size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        px = _mm256_mul_ps(px, _mm256_loadu_ps(values->X + i)); \
        py = _mm256_mul_ps(py, _mm256_loadu_ps(values->Y + i)); \
        pz = _mm256_mul_ps(pz, _mm256_loadu_ps(values->Z + i)); \
        pw = _mm256_mul_ps(pw, _mm256_loadu_ps(values->W + i)); \
    } \
    float x = hprod8_ps(px), y = hprod8_ps(py), z = hprod8_ps(pz), w = hprod8_ps(pw); \
    for (; i < count; ++i) { x *= values->X[i]; y *= values->Y[i]; z *= values->Z[i]; w *= values->W[i]; } \
    return (TYPE){x, y, z, w}; \
} \
TYPE PREFIX##_n_div(const ARRAY *restrict values, size_t count) { \
    if (count == 0) return (TYPE){0}; \
    const float x0 = values->X[0], y0 = values->Y[0], z0 = values->Z[0], w0 = values->W[0]; \
    __m256 px = _mm256_set1_ps(1.0f), py = _mm256_set1_ps(1.0f), pz = _mm256_set1_ps(1.0f), pw = _mm256_set1_ps(1.0f); \
    size_t i = 1; \
    for (; i + 8 <= count; i += 8) { \
        px = _mm256_mul_ps(px, _mm256_loadu_ps(values->X + i)); \
        py = _mm256_mul_ps(py, _mm256_loadu_ps(values->Y + i)); \
        pz = _mm256_mul_ps(pz, _mm256_loadu_ps(values->Z + i)); \
        pw = _mm256_mul_ps(pw, _mm256_loadu_ps(values->W + i)); \
    } \
    float x = hprod8_ps(px), y = hprod8_ps(py), z = hprod8_ps(pz), w = hprod8_ps(pw); \
    for (; i < count; ++i) { x *= values->X[i]; y *= values->Y[i]; z *= values->Z[i]; w *= values->W[i]; } \
    return (TYPE){x0 / x, y0 / y, z0 / z, w0 / w}; \
}

DEFINE_REDUCTIONS_2D(vec2f, vec2f_t, vec2f_arr_t, x, y)
DEFINE_REDUCTIONS_3D(vec3f, vec3f_t, vec3f_arr_t, x, y, z)
DEFINE_REDUCTIONS_4D(vec4f, vec4f_t, vec4f_arr_t, x, y, z, w)

#define DEFINE_BINARY_2D(NAME, INTRIN, OP) \
vec2f_arr_t *vec2f_arr_##NAME(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict a, const vec2f_arr_t *restrict b, size_t count) { \
    size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        _mm256_storeu_ps(out->x + i, INTRIN(_mm256_loadu_ps(a->x + i), _mm256_loadu_ps(b->x + i))); \
        _mm256_storeu_ps(out->y + i, INTRIN(_mm256_loadu_ps(a->y + i), _mm256_loadu_ps(b->y + i))); \
    } \
    for (; i < count; ++i) { out->x[i] = a->x[i] OP b->x[i]; out->y[i] = a->y[i] OP b->y[i]; } \
    return out; \
}
#define DEFINE_BINARY_3D(NAME, INTRIN, OP) \
vec3f_arr_t *vec3f_arr_##NAME(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict a, const vec3f_arr_t *restrict b, size_t count) { \
    size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        _mm256_storeu_ps(out->x + i, INTRIN(_mm256_loadu_ps(a->x + i), _mm256_loadu_ps(b->x + i))); \
        _mm256_storeu_ps(out->y + i, INTRIN(_mm256_loadu_ps(a->y + i), _mm256_loadu_ps(b->y + i))); \
        _mm256_storeu_ps(out->z + i, INTRIN(_mm256_loadu_ps(a->z + i), _mm256_loadu_ps(b->z + i))); \
    } \
    for (; i < count; ++i) { out->x[i] = a->x[i] OP b->x[i]; out->y[i] = a->y[i] OP b->y[i]; out->z[i] = a->z[i] OP b->z[i]; } \
    return out; \
}
#define DEFINE_BINARY_4D(NAME, INTRIN, OP) \
vec4f_arr_t *vec4f_arr_##NAME(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict a, const vec4f_arr_t *restrict b, size_t count) { \
    size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        _mm256_storeu_ps(out->x + i, INTRIN(_mm256_loadu_ps(a->x + i), _mm256_loadu_ps(b->x + i))); \
        _mm256_storeu_ps(out->y + i, INTRIN(_mm256_loadu_ps(a->y + i), _mm256_loadu_ps(b->y + i))); \
        _mm256_storeu_ps(out->z + i, INTRIN(_mm256_loadu_ps(a->z + i), _mm256_loadu_ps(b->z + i))); \
        _mm256_storeu_ps(out->w + i, INTRIN(_mm256_loadu_ps(a->w + i), _mm256_loadu_ps(b->w + i))); \
    } \
    for (; i < count; ++i) { out->x[i] = a->x[i] OP b->x[i]; out->y[i] = a->y[i] OP b->y[i]; out->z[i] = a->z[i] OP b->z[i]; out->w[i] = a->w[i] OP b->w[i]; } \
    return out; \
}

DEFINE_BINARY_2D(add, _mm256_add_ps, +)
DEFINE_BINARY_2D(sub, _mm256_sub_ps, -)
DEFINE_BINARY_2D(mul, _mm256_mul_ps, *)
DEFINE_BINARY_2D(div, _mm256_div_ps, /)
DEFINE_BINARY_3D(add, _mm256_add_ps, +)
DEFINE_BINARY_3D(sub, _mm256_sub_ps, -)
DEFINE_BINARY_3D(mul, _mm256_mul_ps, *)
DEFINE_BINARY_3D(div, _mm256_div_ps, /)
DEFINE_BINARY_4D(add, _mm256_add_ps, +)
DEFINE_BINARY_4D(sub, _mm256_sub_ps, -)
DEFINE_BINARY_4D(mul, _mm256_mul_ps, *)
DEFINE_BINARY_4D(div, _mm256_div_ps, /)

#define DEFINE_SCALE_3D() \
vec3f_arr_t *vec3f_arr_scale(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict values, float scalar, size_t count) { \
    const __m256 scale = _mm256_set1_ps(scalar); size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        _mm256_storeu_ps(out->x + i, _mm256_mul_ps(_mm256_loadu_ps(values->x + i), scale)); \
        _mm256_storeu_ps(out->y + i, _mm256_mul_ps(_mm256_loadu_ps(values->y + i), scale)); \
        _mm256_storeu_ps(out->z + i, _mm256_mul_ps(_mm256_loadu_ps(values->z + i), scale)); \
    } \
    for (; i < count; ++i) { out->x[i] = values->x[i] * scalar; out->y[i] = values->y[i] * scalar; out->z[i] = values->z[i] * scalar; } \
    return out; \
}
#define DEFINE_SCALE_4D() \
vec4f_arr_t *vec4f_arr_scale(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict values, float scalar, size_t count) { \
    const __m256 scale = _mm256_set1_ps(scalar); size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        _mm256_storeu_ps(out->x + i, _mm256_mul_ps(_mm256_loadu_ps(values->x + i), scale)); \
        _mm256_storeu_ps(out->y + i, _mm256_mul_ps(_mm256_loadu_ps(values->y + i), scale)); \
        _mm256_storeu_ps(out->z + i, _mm256_mul_ps(_mm256_loadu_ps(values->z + i), scale)); \
        _mm256_storeu_ps(out->w + i, _mm256_mul_ps(_mm256_loadu_ps(values->w + i), scale)); \
    } \
    for (; i < count; ++i) { out->x[i] = values->x[i] * scalar; out->y[i] = values->y[i] * scalar; out->z[i] = values->z[i] * scalar; out->w[i] = values->w[i] * scalar; } \
    return out; \
}
DEFINE_SCALE_3D()
DEFINE_SCALE_4D()

#define DEFINE_LERP_2D() \
vec2f_arr_t *vec2f_arr_lerp(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict a, const vec2f_arr_t *restrict b, float t, size_t count) { \
    const __m256 vt = _mm256_set1_ps(t); size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        _mm256_storeu_ps(out->x + i, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_loadu_ps(b->x + i), _mm256_loadu_ps(a->x + i)), vt, _mm256_loadu_ps(a->x + i))); \
        _mm256_storeu_ps(out->y + i, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_loadu_ps(b->y + i), _mm256_loadu_ps(a->y + i)), vt, _mm256_loadu_ps(a->y + i))); \
    } \
    for (; i < count; ++i) { out->x[i] = fmaf(b->x[i] - a->x[i], t, a->x[i]); out->y[i] = fmaf(b->y[i] - a->y[i], t, a->y[i]); } \
    return out; \
}
#define DEFINE_LERP_3D() \
vec3f_arr_t *vec3f_arr_lerp(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict a, const vec3f_arr_t *restrict b, float t, size_t count) { \
    const __m256 vt = _mm256_set1_ps(t); size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        _mm256_storeu_ps(out->x + i, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_loadu_ps(b->x + i), _mm256_loadu_ps(a->x + i)), vt, _mm256_loadu_ps(a->x + i))); \
        _mm256_storeu_ps(out->y + i, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_loadu_ps(b->y + i), _mm256_loadu_ps(a->y + i)), vt, _mm256_loadu_ps(a->y + i))); \
        _mm256_storeu_ps(out->z + i, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_loadu_ps(b->z + i), _mm256_loadu_ps(a->z + i)), vt, _mm256_loadu_ps(a->z + i))); \
    } \
    for (; i < count; ++i) { out->x[i] = fmaf(b->x[i] - a->x[i], t, a->x[i]); out->y[i] = fmaf(b->y[i] - a->y[i], t, a->y[i]); out->z[i] = fmaf(b->z[i] - a->z[i], t, a->z[i]); } \
    return out; \
}
#define DEFINE_LERP_4D() \
vec4f_arr_t *vec4f_arr_lerp(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict a, const vec4f_arr_t *restrict b, float t, size_t count) { \
    const __m256 vt = _mm256_set1_ps(t); size_t i = 0; \
    for (; i + 8 <= count; i += 8) { \
        _mm256_storeu_ps(out->x + i, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_loadu_ps(b->x + i), _mm256_loadu_ps(a->x + i)), vt, _mm256_loadu_ps(a->x + i))); \
        _mm256_storeu_ps(out->y + i, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_loadu_ps(b->y + i), _mm256_loadu_ps(a->y + i)), vt, _mm256_loadu_ps(a->y + i))); \
        _mm256_storeu_ps(out->z + i, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_loadu_ps(b->z + i), _mm256_loadu_ps(a->z + i)), vt, _mm256_loadu_ps(a->z + i))); \
        _mm256_storeu_ps(out->w + i, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_loadu_ps(b->w + i), _mm256_loadu_ps(a->w + i)), vt, _mm256_loadu_ps(a->w + i))); \
    } \
    for (; i < count; ++i) { out->x[i] = fmaf(b->x[i] - a->x[i], t, a->x[i]); out->y[i] = fmaf(b->y[i] - a->y[i], t, a->y[i]); out->z[i] = fmaf(b->z[i] - a->z[i], t, a->z[i]); out->w[i] = fmaf(b->w[i] - a->w[i], t, a->w[i]); } \
    return out; \
}
DEFINE_LERP_2D()
DEFINE_LERP_3D()
DEFINE_LERP_4D()
