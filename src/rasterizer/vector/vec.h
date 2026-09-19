#pragma once

#include <stdint.h>
#include <stddef.h>
#include <immintrin.h>

typedef struct vec2f_t {
    float x, y;
} vec2f_t;

typedef struct vec3f_t {
    float x, y, z;
} vec3f_t;

typedef struct vec4f_t {
    float x, y, z, w;
} vec4f_t;

typedef struct vertex_t {
    vec4f_t pos, color;
} vertex_t;

typedef struct triangle_t {
    vertex_t v0, v1, v2;
} triangle_t;

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
vec2f_t vec2f_add(const vec2f_t a, const vec2f_t b);
vec2f_t vec2f_add3(const vec2f_t a, const vec2f_t b, const vec2f_t c);
vec2f_t vec2f_mul(const vec2f_t a, const vec2f_t b);
vec2f_t vec2f_mul3(const vec2f_t a, const vec2f_t b, const vec2f_t c);
vec2f_t vec2f_div(const vec2f_t a, const vec2f_t b);
vec2f_t vec2f_div3(const vec2f_t a, const vec2f_t b, const vec2f_t c);
vec2f_t vec2f_sub(const vec2f_t a, const vec2f_t b);
vec2f_t vec2f_sub3(const vec2f_t a, const vec2f_t b, const vec2f_t c);
float vec2f_dot(const vec2f_t a, const vec2f_t b);
float vec2f_dot3(const vec2f_t a, const vec2f_t b, const vec2f_t c);
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

// vec3f_t math
vec3f_t vec3f_add(const vec3f_t a, const vec3f_t b);
vec3f_t vec3f_add3(const vec3f_t a, const vec3f_t b, const vec3f_t c);
vec3f_t vec3f_mul(const vec3f_t a, const vec3f_t b);
vec3f_t vec3f_mul3(const vec3f_t a, const vec3f_t b, const vec3f_t c);
vec3f_t vec3f_div(const vec3f_t a, const vec3f_t b);
vec3f_t vec3f_div3(const vec3f_t a, const vec3f_t b, const vec3f_t c);
vec3f_t vec3f_sub(const vec3f_t a, const vec3f_t b);
vec3f_t vec3f_sub3(const vec3f_t a, const vec3f_t b, const vec3f_t c);
float vec3f_dot(const vec3f_t a, const vec3f_t b);
float vec3f_dot3(const vec3f_t a, const vec3f_t b, const vec3f_t c);
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

// vec4f_t math
vec4f_t vec4f_add(const vec4f_t a, const vec4f_t b);
vec4f_t vec4f_add3(const vec4f_t a, const vec4f_t b, const vec4f_t c);
vec4f_t vec4f_mul(const vec4f_t a, const vec4f_t b);
vec4f_t vec4f_mul3(const vec4f_t a, const vec4f_t b, const vec4f_t c);
vec4f_t vec4f_div(const vec4f_t a, const vec4f_t b);
vec4f_t vec4f_div3(const vec4f_t a, const vec4f_t b, const vec4f_t c);
vec4f_t vec4f_sub(const vec4f_t a, const vec4f_t b);
vec4f_t vec4f_sub3(const vec4f_t a, const vec4f_t b, const vec4f_t c);
float vec4f_dot(const vec4f_t a, const vec4f_t b);
float vec4f_dot3(const vec4f_t a, const vec4f_t b, const vec4f_t c);
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