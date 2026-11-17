#pragma once

#include "../vector/vec.h"
#include <math.h>

/**
 * @brief 4x4 float matrix, row-major: m[row][col]
 * @note vectors are treated as column vectors, so out = M * v
 *       and mat4f_mul(a, b) means "apply b first, then a"
 */
typedef struct mat4f_t {
    float m[4][4];
} mat4f_t;

static inline mat4f_t mat4f_identity(void) {
    mat4f_t r = {{{0}}};
    r.m[0][0] = 1.0f;
    r.m[1][1] = 1.0f;
    r.m[2][2] = 1.0f;
    r.m[3][3] = 1.0f;
    return r;
}

static inline mat4f_t mat4f_mul(const mat4f_t a, const mat4f_t b) {
    mat4f_t r = {{{0}}};

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            r.m[row][col] = a.m[row][0] * b.m[0][col]
                          + a.m[row][1] * b.m[1][col]
                          + a.m[row][2] * b.m[2][col]
                          + a.m[row][3] * b.m[3][col];
        }
    }
    return r;
}

static inline vec4f_t mat4f_mul_vec4f(const mat4f_t *mat, const vec4f_t v) {
    return VEC_INIT(vec4f_t,
        mat->m[0][0] * v.x + mat->m[0][1] * v.y + mat->m[0][2] * v.z + mat->m[0][3] * v.w,
        mat->m[1][0] * v.x + mat->m[1][1] * v.y + mat->m[1][2] * v.z + mat->m[1][3] * v.w,
        mat->m[2][0] * v.x + mat->m[2][1] * v.y + mat->m[2][2] * v.z + mat->m[2][3] * v.w,
        mat->m[3][0] * v.x + mat->m[3][1] * v.y + mat->m[3][2] * v.z + mat->m[3][3] * v.w
    );
}

static inline mat4f_t mat4f_translate(const float x, const float y, const float z) {
    mat4f_t r = mat4f_identity();
    r.m[0][3] = x;
    r.m[1][3] = y;
    r.m[2][3] = z;
    return r;
}

static inline mat4f_t mat4f_scale(const float s) {
    mat4f_t r = mat4f_identity();
    r.m[0][0] = s;
    r.m[1][1] = s;
    r.m[2][2] = s;
    return r;
}

/**
 * @param angle in radians
 */
static inline mat4f_t mat4f_rotate_x(const float angle) {
    const float c = cosf(angle);
    const float s = sinf(angle);
    mat4f_t r = mat4f_identity();
    r.m[1][1] = c; r.m[1][2] = -s;
    r.m[2][1] = s; r.m[2][2] = c;
    return r;
}

/**
 * @param angle in radians
 */
static inline mat4f_t mat4f_rotate_y(const float angle) {
    const float c = cosf(angle);
    const float s = sinf(angle);
    mat4f_t r = mat4f_identity();
    r.m[0][0] = c;  r.m[0][2] = s;
    r.m[2][0] = -s; r.m[2][2] = c;
    return r;
}

/**
 * @brief OpenGL style perspective projection (right handed, camera looks down -z)
 * @param fov_y vertical field of view in radians
 * @param aspect width / heigth
 * @param near near plane distance, must be > 0
 * @param far far plane distance, must be > near
 * @note after the perspective divide z lands in [-1, 1], -1 = near plane
 */
static inline mat4f_t mat4f_perspective(const float fov_y, const float aspect, const float near, const float far) {
    const float f = 1.0f / tanf(fov_y * 0.5f);
    mat4f_t r = {{{0}}};
    r.m[0][0] = f / aspect;
    r.m[1][1] = f;
    r.m[2][2] = (far + near) / (near - far);
    r.m[2][3] = (2.0f * far * near) / (near - far);
    r.m[3][2] = -1.0f;
    return r;
}
