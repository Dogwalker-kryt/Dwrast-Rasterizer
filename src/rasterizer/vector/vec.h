#pragma once

#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>
#include <math.h>

#ifdef __cplusplus
#define VEC_INIT(type, ...) type{__VA_ARGS__}
#else
#define VEC_INIT(type, ...) (type){__VA_ARGS__}
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define restrict __restrict

typedef struct vec2f_t {
    float x, y;
} vec2f_t;

typedef struct vec3f_t {
    float x, y, z;
} vec3f_t;

typedef struct vec4f_t {
    float x, y, z, w;
} vec4f_t;

// SoA
typedef struct vec2f_arr_t {
    float *x;
    float *y;
} vec2f_arr_t;

typedef struct vec3f_arr_t {
    float *x;
    float *y;
    float *z;
} vec3f_arr_t;

typedef struct vec4f_arr_t {
    float *x;
    float *y;
    float *z;
    float *w;
} vec4f_arr_t;

typedef struct rgba_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} rgba_t;


/**
 * @brief same as vec3f_t; compatible with vec4f_t Arithmetic functions
 */
typedef vec3f_t barycentric_t;

static inline float hsum4(__m128 v) {
    __m128 t = _mm_add_ps(v, _mm_movehl_ps(v, v));
    t = _mm_add_ss(t, _mm_shuffle_ps(t, t, 1));
    return _mm_cvtss_f32(t);
}

static inline float hsum4_ps(__m128 v) {
    __m128 hi = _mm_movehl_ps(v, v);
    v = _mm_add_ps(v, hi);

    __m128 shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
    v = _mm_add_ss(v, shuf);

    return _mm_cvtss_f32(v);
}

static inline float hmul4(__m128 v) {
    __m128 hi = _mm_movehl_ps(v, v);
    v = _mm_mul_ps(v, hi);

    __m128 shuf = _mm_shuffle_ps(v, v, 1);
    v = _mm_mul_ss(v, shuf);

    return _mm_cvtss_f32(v);
}


inline vec2f_t from_vec2_arr(const vec2f_arr_t *arr, size_t n);
inline vec3f_t from_vec3_arr(const vec3f_arr_t *arr, size_t n);
inline vec4f_t from_vec4_arr(const vec4f_arr_t *arr, size_t n);

vec2f_t *vec2f_arr_to_aos(const vec2f_arr_t *restrict arr, vec2f_t *restrict out, size_t count);
vec3f_t *vec3f_arr_to_aos(const vec3f_arr_t *restrict arr, vec3f_t *restrict out, size_t count);
vec4f_t *vec4f_arr_to_aos(const vec4f_arr_t *restrict arr, vec4f_t *restrict out, size_t count);

// vec2f_t math

static inline vec2f_t vec2f_add(const vec2f_t a, const vec2f_t b) {
    return VEC_INIT(vec2f_t,
        a.x + b.x, 
        a.y + b.y
    );
}

static inline vec2f_t vec2f_add_scalar(const vec2f_t a, const float scalar) {
    return VEC_INIT(vec2f_t,
        a.x + scalar, 
        a.y + scalar
    );
}

static inline vec2f_t vec2f_add3(const vec2f_t a, const vec2f_t b, const vec2f_t c) {
    return VEC_INIT(vec2f_t,
        a.x + b.x + c.x, 
        a.y + b.y + c.y
    );
}

static inline vec2f_t vec2f_mul(const vec2f_t a, const vec2f_t b) {
    return VEC_INIT(vec2f_t,
        a.x * b.x, 
        a.y * b.y
    );
}

static inline vec2f_t vec2f_mul_scalar(const vec2f_t a, const float scalar) {
    return VEC_INIT(vec2f_t,
        a.x * scalar, 
        a.y * scalar
    );
}

static inline vec2f_t vec2f_mul3(const vec2f_t a, const vec2f_t b, const vec2f_t c) {
    return VEC_INIT(vec2f_t,
        a.x * b.x * c.x, 
        a.y * b.y * c.y
    );
}

static inline vec2f_t vec2f_div(const vec2f_t a, const vec2f_t b) {
    return VEC_INIT(vec2f_t,
        a.x / b.x, 
        a.y / b.y
    );
}

static inline vec2f_t vec2f_div_scalar(const vec2f_t a, const float scalar) {
    return VEC_INIT(vec2f_t,
        a.x / scalar, 
        a.y / scalar
    );
}

static inline vec2f_t vec2f_div3(const vec2f_t a, const vec2f_t b, const vec2f_t c) {
    return VEC_INIT(vec2f_t,
        a.x / b.x / c.x, 
        a.y / b.y / c.y
    );
}

static inline vec2f_t vec2f_sub(const vec2f_t a, const vec2f_t b) {
    return VEC_INIT(vec2f_t,
        a.x - b.x, 
        a.y - b.y
    );
}

static inline vec2f_t vec2f_sub_scalar(const vec2f_t a, const float scalar) {
    return VEC_INIT(vec2f_t,
        a.x - scalar, 
        a.y - scalar
    );
}

static inline vec2f_t vec2f_sub3(const vec2f_t a, const vec2f_t b, const vec2f_t c) {
    return VEC_INIT(vec2f_t,
        a.x - b.x - c.x, 
        a.y - b.y - c.y
    );
}

static inline float vec2f_dot(const vec2f_t a, const vec2f_t b) {
    return a.x * b.x + a.y * b.y;
}

static inline float vec2f_dot3(const vec2f_t a, const vec2f_t b, const vec2f_t c) {
    return a.x * b.x * c.x + a.y * b.y * c.y;
}

static inline float vec2f_cross(const vec2f_t a, const vec2f_t b) {
    return a.x * b.y - a.y * b.x;
}

static inline float vec2f_edge(const vec2f_t a, const vec2f_t b, const vec2f_t p) {
    return vec2f_cross(vec2f_sub(b, a), vec2f_sub(p, a));
}

/**
 * @brief overload for vec2f_sub
 */
static inline vec2f_t edge_vector(const vec2f_t a, const vec2f_t b) {
    return vec2f_sub(a, b);
}

__always_inline float vec2f_length_squared(const vec2f_t vec) {
    return vec.x * vec.x + vec.y * vec.y;
}

__always_inline float vec2f_length(const vec2f_t vec) {
    return sqrtf(vec2f_length_squared(vec));
}

static inline vec2f_t vec2f_normalize(const vec2f_t vec) {
    float len = vec2f_length(vec);

    return VEC_INIT(vec2f_t,
        vec.x / len,
        vec.y / len
    );
}

vec2f_t vec2f_n_add(const vec2f_arr_t *restrict vec_array, size_t count);
/**
 * @brief Subtracts an array of vec2f_t vectors and returns the result.
 * @warning count must be at least 1, otherwise the behavior is undefined.
 */
vec2f_t vec2f_n_sub(const vec2f_arr_t *restrict vec_array, size_t count);
vec2f_t vec2f_n_mul(const vec2f_arr_t *restrict vec_array, size_t count);
/**
 * @brief Divides an array of vec2f_t vectors and returns the result.
 * @warning count must be at least 1, otherwise the behavior is undefined.
 */
vec2f_t vec2f_n_div(const vec2f_arr_t *restrict vec_array, size_t count);
vec2f_arr_t *vec2f_arr_add(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict a, const vec2f_arr_t *restrict b, size_t count);
vec2f_arr_t *vec2f_arr_sub(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict a, const vec2f_arr_t *restrict b, size_t count);
vec2f_arr_t *vec2f_arr_mul(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict a, const vec2f_arr_t *restrict b, size_t count);
vec2f_arr_t *vec2f_arr_div(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict a, const vec2f_arr_t *restrict b, size_t count);
static inline vec2f_t vec2f_lerp(vec2f_t a, vec2f_t b, float t) {
    return VEC_INIT(vec2f_t,
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t
    );
}


// vec3f_t math

static inline vec3f_t vec3f_add(const vec3f_t a, const vec3f_t b) {
    return VEC_INIT(vec3f_t,
        a.x + b.x, 
        a.y + b.y, 
        a.z + b.z
    );
}

static inline vec3f_t vec3f_add3(const vec3f_t a, const vec3f_t b, const vec3f_t c) {
    return VEC_INIT(vec3f_t,
        a.x + b.x + c.x, 
        a.y + b.y + c.y, 
        a.z + b.z + c.z
    );
}

static inline vec3f_t vec3f_mul(const vec3f_t a, const vec3f_t b) {
    return VEC_INIT(vec3f_t,
        a.x * b.x, 
        a.y * b.y, 
        a.z * b.z
    );
}

static inline vec3f_t vec3f_mul3(const vec3f_t a, const vec3f_t b, const vec3f_t c) {
    return VEC_INIT(vec3f_t,
        a.x * b.x * c.x, 
        a.y * b.y * c.y, 
        a.z * b.z * c.z
    );
}

static inline vec3f_t vec3f_div(const vec3f_t a, const vec3f_t b) {
    return VEC_INIT(vec3f_t,
        a.x / b.x, 
        a.y / b.y, 
        a.z / b.z
    );
}

static inline vec3f_t vec3f_div3(const vec3f_t a, const vec3f_t b, const vec3f_t c) {
    return VEC_INIT(vec3f_t,
        a.x / b.x / c.x, 
        a.y / b.y / c.y, 
        a.z / b.z / c.z
    );
}

static inline vec3f_t vec3f_sub(const vec3f_t a, const vec3f_t b) {
    return VEC_INIT(vec3f_t,
        a.x - b.x, 
        a.y - b.y, 
        a.z - b.z
    );
}

static inline vec3f_t vec3f_sub3(const vec3f_t a, const vec3f_t b, const vec3f_t c) {
    return VEC_INIT(vec3f_t,
        a.x - b.x - c.x, 
        a.y - b.y - c.y, 
        a.z - b.z - c.z
    );
}

static inline float vec3f_dot(const vec3f_t a, const vec3f_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float vec3f_dot3(const vec3f_t a, const vec3f_t b, const vec3f_t c) {
    return a.x * b.x * c.x + a.y * b.y * c.y + a.z * b.z * c.z;
}

__always_inline float vec3f_length_squared(const vec3f_t vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

__always_inline float vec3f_length(const vec3f_t vec) {
    return sqrtf(vec3f_length(vec));
}

static inline vec3f_t vec3f_normalize(const vec3f_t vec) {
    float len = vec3f_length_squared(vec);

    return VEC_INIT(vec3f_t,
        vec.x / len,
        vec.y / len,
        vec.z / len
    );
}

vec3f_t vec3f_n_add(const vec3f_arr_t *restrict vec_array, size_t count);
/**
 * @brief Subtracts an array of vec3f_t vectors and returns the result. 
 * @warning count must be at least 1, otherwise the behavior is undefined.
 */
vec3f_t vec3f_n_sub(const vec3f_arr_t *restrict vec_array, size_t count);
vec3f_t vec3f_n_mul(const vec3f_arr_t *restrict vec_array, size_t count);
/**
 * @brief Divides an array of vec3f_t vectors and returns the result.
 * @warning count must be at least 1, otherwise the behavior is undefined.
 */
vec3f_t vec3f_n_div(const vec3f_arr_t *restrict vec_array, size_t count);
vec3f_arr_t *vec3f_arr_add(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict a, const vec3f_arr_t *restrict b, size_t count);
vec3f_arr_t *vec3f_arr_sub(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict a, const vec3f_arr_t *restrict b, size_t count);
vec3f_arr_t *vec3f_arr_mul(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict a, const vec3f_arr_t *restrict b, size_t count);
vec3f_arr_t *vec3f_arr_div(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict a, const vec3f_arr_t *restrict b, size_t count);
vec3f_arr_t *vec3f_arr_scale(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict vec_array, float s, size_t count);
static inline vec3f_t vec3f_lerp(vec3f_t a, vec3f_t b, float t) {
    return VEC_INIT(vec3f_t,
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    );
}

// vec4f_t math

static inline vec4f_t vec4f_add(const vec4f_t a, const vec4f_t b) {
    return VEC_INIT(vec4f_t,
        a.x + b.x, 
        a.y + b.y, 
        a.z + b.z, 
        a.w + b.w
    );
}

static inline vec4f_t vec4f_add3(const vec4f_t a, const vec4f_t b, const vec4f_t c) {
    return VEC_INIT(vec4f_t,
        a.x + b.x + c.x, 
        a.y + b.y + c.y, 
        a.z + b.z + c.z, 
        a.w + b.w + c.w
    );
}

static inline vec4f_t vec4f_mul(const vec4f_t a, const vec4f_t b) {
    return VEC_INIT(vec4f_t,
        a.x * b.x, 
        a.y * b.y, 
        a.z * b.z, 
        a.w * b.w
    );
}

static inline vec4f_t vec4f_mul3(const vec4f_t a, const vec4f_t b, const vec4f_t c) {
    return VEC_INIT(vec4f_t,
        a.x * b.x * c.x, 
        a.y * b.y * c.y, 
        a.z * b.z * c.z, 
        a.w * b.w * c.w
    );
}

static inline vec4f_t vec4f_div(const vec4f_t a, const vec4f_t b) {
    return VEC_INIT(vec4f_t,
        a.x / b.x, 
        a.y / b.y, 
        a.z / b.z, 
        a.w / b.w
    );
}

static inline vec4f_t vec4f_div3(const vec4f_t a, const vec4f_t b, const vec4f_t c) {
    return VEC_INIT(vec4f_t,
        a.x / b.x / c.x, 
        a.y / b.y / c.y, 
        a.z / b.z / c.z, 
        a.w / b.w / c.w
    );
}

static inline vec4f_t vec4f_sub(const vec4f_t a, const vec4f_t b) {
    return VEC_INIT(vec4f_t,
        a.x - b.x, 
        a.y - b.y, 
        a.z - b.z, 
        a.w - b.w
    );
}

static inline vec4f_t vec4f_sub3(const vec4f_t a, const vec4f_t b, const vec4f_t c) {
    return VEC_INIT(vec4f_t,
        a.x - b.x - c.x, 
        a.y - b.y - c.y, 
        a.z - b.z - c.z, 
        a.w - b.w - c.w
    );
}

static inline float vec4f_dot(const vec4f_t a, const vec4f_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static inline float vec4f_dot3(const vec4f_t a, const vec4f_t b, const vec4f_t c) {
    return a.x * b.x * c.x + a.y * b.y * c.y + a.z * b.z * c.z + a.w * b.w * c.w;
}

__always_inline float vec4f_length_squared(const vec4f_t vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
}

__always_inline float vec4f_length(const vec4f_t vec) {
    return sqrtf(vec4f_length_squared(vec));
}

static inline vec4f_t vec4f_normalize(const vec4f_t vec) {
    float len = vec4f_length(vec);

    return VEC_INIT(vec4f_t,
        vec.x / len,
        vec.y / len,
        vec.z / len,
        vec.w / len
    );
}

vec4f_t vec4f_n_add(const vec4f_arr_t *restrict vec_array, size_t count);
/**
 * @brief Subtracts an array of vec4f_t vectors and returns the result.
 * @warning count must be at least 1, otherwise the behavior is undefined.
 */
vec4f_t vec4f_n_sub(const vec4f_arr_t *restrict vec_array, size_t count);
vec4f_t vec4f_n_mul(const vec4f_arr_t *restrict vec_array, size_t count);
/**
 * @brief Divides an array of vec4f_t vectors and returns the result.
 * @warning count must be at least 1, otherwise the behavior is undefined.
 */
vec4f_t vec4f_n_div(const vec4f_arr_t *restrict vec_array, size_t count);
vec4f_arr_t *vec4f_arr_add(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict a, const vec4f_arr_t *restrict b, size_t count);
vec4f_arr_t *vec4f_arr_sub(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict a, const vec4f_arr_t *restrict b, size_t count);
vec4f_arr_t *vec4f_arr_mul(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict a, const vec4f_arr_t *restrict b, size_t count);
vec4f_arr_t *vec4f_arr_div(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict a, const vec4f_arr_t *restrict b, size_t count);
vec4f_arr_t *vec4f_arr_scale(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict vec_array, float s, size_t count);
static inline vec4f_t vec4f_lerp(vec4f_t a, vec4f_t b, float t) {
    __m128 va = _mm_loadu_ps(&a.x);
    __m128 vb = _mm_loadu_ps(&b.x);
    __m128 vt = _mm_set1_ps(t);

    __m128 out = _mm_add_ps(va, _mm_mul_ps(_mm_sub_ps(vb, va), vt));

    vec4f_t r;
    _mm_storeu_ps(&r.x, out);
    return r;
}

static inline vec4f_t vec4f_scale(const vec4f_t a, const float t) {
    const __m128 scale = _mm_set1_ps(t);
    const __m128 va = _mm_loadu_ps(&a.x);

    vec4f_t res;
    _mm_storeu_ps(&res.x, _mm_mul_ps(va, scale));

    return res;
}

#ifdef __cplusplus
}
#endif