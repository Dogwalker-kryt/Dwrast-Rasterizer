#include "vec.h"

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
    for (size_t i = 0; i < count; ++i) {
        out[i] = (vec2f_t){arr->x[i], arr->y[i]};
    }
    return out;
}

vec3f_t *vec3f_arr_to_aos(const vec3f_arr_t *restrict arr, vec3f_t *restrict out, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        out[i] = (vec3f_t){arr->x[i], arr->y[i], arr->z[i]};
    }
    return out;
}

vec4f_t *vec4f_arr_to_aos(const vec4f_arr_t *restrict arr, vec4f_t *restrict out, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        out[i] = (vec4f_t){arr->x[i], arr->y[i], arr->z[i], arr->w[i]};
    }
    return out;
}

// ----------------------------------------------------------------------------------------------------------------------------
// vec2

vec2f_t vec2f_add(const vec2f_t a, const vec2f_t b) {
    return (vec2f_t){a.x + b.x, a.y + b.y};
}

vec2f_t vec2f_add3(const vec2f_t a, const vec2f_t b, const vec2f_t c) {
    return (vec2f_t){a.x + b.x + c.x, a.y + b.y + c.y};
}

vec2f_t vec2f_mul(const vec2f_t a, const vec2f_t b) {
    return (vec2f_t){a.x * b.x, a.y * b.y};
}

vec2f_t vec2f_mul3(const vec2f_t a, const vec2f_t b, const vec2f_t c) {
    return (vec2f_t){a.x * b.x * c.x, a.y * b.y * c.y};
}

vec2f_t vec2f_div(const vec2f_t a, const vec2f_t b) {
    return (vec2f_t){a.x / b.x, a.y / b.y};
}

vec2f_t vec2f_div3(const vec2f_t a, const vec2f_t b, const vec2f_t c) {
    return (vec2f_t){a.x / b.x / c.x, a.y / b.y / c.y};
}

vec2f_t vec2f_sub(const vec2f_t a, const vec2f_t b) {
    return (vec2f_t){a.x - b.x, a.y - b.y};
}

vec2f_t vec2f_sub3(const vec2f_t a, const vec2f_t b, const vec2f_t c) {
    return (vec2f_t){a.x - b.x - c.x, a.y - b.y - c.y};
}

float vec2f_dot(const vec2f_t a, const vec2f_t b) {
    return a.x * b.x + a.y * b.y;
}

float vec2f_dot3(const vec2f_t a, const vec2f_t b, const vec2f_t c) {
    return a.x * b.x * c.x + a.y * b.y * c.y;
}

vec2f_t vec2f_n_add(const vec2f_arr_t *restrict vec_array, size_t count) {
    vec2f_t result = {0.0f, 0.0f};

    size_t i = 0;
    __m128 sumx = _mm_setzero_ps();
    __m128 sumy = _mm_setzero_ps();

    for (; i + 4 <= count; i += 4) {
        sumx = _mm_add_ps(sumx, _mm_loadu_ps(&vec_array->x[i]));
        sumy = _mm_add_ps(sumy, _mm_loadu_ps(&vec_array->y[i]));
    }

    result.x = hsum4_ps(sumx);
    result.y = hsum4_ps(sumy);

    for (; i < count; ++i) {
        result.x += vec_array->x[i];
        result.y += vec_array->y[i];
    }

    return result;
}

vec2f_t vec2f_n_sub(const vec2f_arr_t *restrict vec_array, size_t count) {
    const float x0 = vec_array->x[0];
    const float y0 = vec_array->y[0];

    __m128 sx = _mm_setzero_ps();
    __m128 sy = _mm_setzero_ps();

    size_t i = 1;

    for (; i + 4 <= count; i += 4) {
        sx = _mm_add_ps(sx, _mm_loadu_ps(vec_array->x + i));
        sy = _mm_add_ps(sy, _mm_loadu_ps(vec_array->y + i));
    }

    float tail_x = 0.0f;
    float tail_y = 0.0f;

    for (; i < count; ++i) {
        tail_x += vec_array->x[i];
        tail_y += vec_array->y[i];
    }

    return (vec2f_t){
        x0 - (hsum4_ps(sx) + tail_x),
        y0 - (hsum4_ps(sy) + tail_y)
    }; 
}

vec2f_t vec2f_n_mul(const vec2f_arr_t *restrict vec_array, size_t count) {
    size_t i = 0;
    __m128 prodx = _mm_set1_ps(1.0f);
    __m128 prody = _mm_set1_ps(1.0f);

    for (; i + 4 <= count; i += 4) {
        prodx = _mm_mul_ps(prodx, _mm_loadu_ps(&vec_array->x[i]));
        prody = _mm_mul_ps(prody, _mm_loadu_ps(&vec_array->y[i]));
    }

    vec2f_t result = { hsum4_ps(prodx), hsum4_ps(prody) };

    for (; i < count; ++i) {
        result.x *= vec_array->x[i];
        result.y *= vec_array->y[i];
    }

    return result;
}

vec2f_t vec2f_n_div(const vec2f_arr_t *restrict vec_array, size_t count) {
    const float x0 = vec_array->x[0];
    const float y0 = vec_array->y[0];

    __m128 sx = _mm_set1_ps(1.0f);
    __m128 sy = _mm_set1_ps(1.0f);

    size_t i = 1;

    for (; i + 4 <= count; i += 4) {
        sx = _mm_mul_ps(sx, _mm_loadu_ps(vec_array->x + i));
        sy = _mm_mul_ps(sy, _mm_loadu_ps(vec_array->y + i));
    }

    float tail_x = 1.0f;
    float tail_y = 1.0f;

    for (; i < count; ++i) {
        tail_x *= vec_array->x[i];
        tail_y *= vec_array->y[i];
    }

    return (vec2f_t){
        x0 / (hmul4(sx) * tail_x),
        y0 / (hmul4(sy) * tail_y)
    };
}

vec2f_arr_t *vec2f_arr_add(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict a, const vec2f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);

        _mm_storeu_ps(&out->x[i], _mm_add_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_add_ps(ay, by));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] + b->x[i];
        out->y[i] = a->y[i] + b->y[i];
    }

    return out;
}

vec2f_arr_t *vec2f_arr_sub(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict a, const vec2f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);

        _mm_storeu_ps(&out->x[i], _mm_sub_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_sub_ps(ay, by));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] - b->x[i];
        out->y[i] = a->y[i] - b->y[i];
    }

    return out;
}

vec2f_arr_t *vec2f_arr_mul(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict a, const vec2f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);

        _mm_storeu_ps(&out->x[i], _mm_mul_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_mul_ps(ay, by));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] * b->x[i];
        out->y[i] = a->y[i] * b->y[i];
    }

    return out;
}

vec2f_arr_t *vec2f_arr_div(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict a, const vec2f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);

        _mm_storeu_ps(&out->x[i], _mm_div_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_div_ps(ay, by));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] / b->x[i];
        out->y[i] = a->y[i] / b->y[i];
    }

    return out;
}

vec2f_arr_t *vec2f_arr_scale(vec2f_arr_t *restrict out, const vec2f_arr_t *restrict vec_array, float s, size_t count) {
    const __m128 scale = _mm_set1_ps(s);

    size_t i = 0;

    for (; i + 4 <= count; i += 4) {
        __m128 x = _mm_loadu_ps(&vec_array->x[i]);
        __m128 y = _mm_loadu_ps(&vec_array->y[i]);

        _mm_storeu_ps(
            &out->x[i],
            _mm_mul_ps(x, scale)
        );

        _mm_storeu_ps(
            &out->y[i],
            _mm_mul_ps(y, scale)
        );
    }

    for (; i < count; ++i) {
        out->x[i] = vec_array->x[i] * s;
        out->y[i] = vec_array->y[i] * s;
    }

    return out;
}

// ----------------------------------------------------------------------------------------------------------------------------
// vec3

vec3f_t vec3f_add(const vec3f_t a, const vec3f_t b) {
    return (vec3f_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3f_t vec3f_add3(const vec3f_t a, const vec3f_t b, const vec3f_t c) {
    return (vec3f_t){a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z};
}

vec3f_t vec3f_mul(const vec3f_t a, const vec3f_t b) {
    return (vec3f_t){a.x * b.x, a.y * b.y, a.z * b.z};
}

vec3f_t vec3f_mul3(const vec3f_t a, const vec3f_t b, const vec3f_t c) {
    return (vec3f_t){a.x * b.x * c.x, a.y * b.y * c.y, a.z * b.z * c.z};
}

vec3f_t vec3f_div(const vec3f_t a, const vec3f_t b) {
    return (vec3f_t){a.x / b.x, a.y / b.y, a.z / b.z};
}

vec3f_t vec3f_div3(const vec3f_t a, const vec3f_t b, const vec3f_t c) {
    return (vec3f_t){a.x / b.x / c.x, a.y / b.y / c.y, a.z / b.z / c.z};
}

vec3f_t vec3f_sub(const vec3f_t a, const vec3f_t b) {
    return (vec3f_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3f_t vec3f_sub3(const vec3f_t a, const vec3f_t b, const vec3f_t c) {
    return (vec3f_t){a.x - b.x - c.x, a.y - b.y - c.y, a.z - b.z - c.z};
}

float vec3f_dot(const vec3f_t a, const vec3f_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vec3f_dot3(const vec3f_t a, const vec3f_t b, const vec3f_t c) {
    return a.x * b.x * c.x + a.y * b.y * c.y + a.z * b.z * c.z;
}

vec3f_t vec3f_n_add(const vec3f_arr_t *restrict vec_array, size_t count) {
    vec3f_t result = {0.0f, 0.0f};

    size_t i = 0;
    __m128 sumx = _mm_setzero_ps();
    __m128 sumy = _mm_setzero_ps();
    __m128 sumz = _mm_setzero_ps();

    for (; i + 4 <= count; i += 4) {
        sumx = _mm_add_ps(sumx, _mm_loadu_ps(&vec_array->x[i]));
        sumy = _mm_add_ps(sumy, _mm_loadu_ps(&vec_array->y[i]));
        sumz = _mm_add_ps(sumz, _mm_loadu_ps(&vec_array->z[i]));
    }

    result.x = hsum4_ps(sumx);
    result.y = hsum4_ps(sumy);
    result.z = hsum4_ps(sumz);

    for (; i < count; ++i) {
        result.x += vec_array->x[i];
        result.y += vec_array->y[i];
        result.z += vec_array->z[i];
    }

    return result;
}

vec3f_t vec3f_n_sub(const vec3f_arr_t *restrict vec_array, size_t count) {
    const float x0 = vec_array->x[0];
    const float y0 = vec_array->y[0];
    const float z0 = vec_array->z[0];

    __m128 sx = _mm_setzero_ps();
    __m128 sy = _mm_setzero_ps();
    __m128 sz = _mm_setzero_ps();

    size_t i = 1;

    for (; i + 4 <= count; i += 4) {
        sx = _mm_add_ps(sx, _mm_loadu_ps(vec_array->x + i));
        sy = _mm_add_ps(sy, _mm_loadu_ps(vec_array->y + i));
        sz = _mm_add_ps(sz, _mm_loadu_ps(vec_array->z + i));
    }

    float tail_x = 0.0f;
    float tail_y = 0.0f;
    float tail_z = 0.0f;

    for (; i < count; ++i) {
        tail_x += vec_array->x[i];
        tail_y += vec_array->y[i];
        tail_z += vec_array->z[i];
    }

    return (vec3f_t){
        x0 - (hsum4_ps(sx) + tail_x),
        y0 - (hsum4_ps(sy) + tail_y),
        z0 - (hsum4_ps(sz) + tail_z)
    }; 
}

vec3f_t vec3f_n_mul(const vec3f_arr_t *restrict vec_array, size_t count) {
    vec3f_t result = {1.0f, 1.0f, 1.0f};

    for (size_t i = 0; i < count; ++i) {
        result.x *= vec_array->x[i];
        result.y *= vec_array->y[i];
        result.z *= vec_array->z[i];
    }

    return result;
}

vec3f_t vec3f_n_div(const vec3f_arr_t *restrict vec_array, size_t count) {
    const float x0 = vec_array->x[0];
    const float y0 = vec_array->y[0];
    const float z0 = vec_array->z[0];

    __m128 sx = _mm_set1_ps(1.0f);
    __m128 sy = _mm_set1_ps(1.0f);
    __m128 sz = _mm_set1_ps(1.0f);

    size_t i = 1;

    for (; i + 4 <= count; i += 4) {
        sx = _mm_mul_ps(sx, _mm_loadu_ps(vec_array->x + i));
        sy = _mm_mul_ps(sy, _mm_loadu_ps(vec_array->y + i));
        sz = _mm_mul_ps(sz, _mm_loadu_ps(vec_array->z + i));
    }

    float tail_x = 1.0f;
    float tail_y = 1.0f;
    float tail_z = 1.0f;

    for (; i < count; ++i) {
        tail_x *= vec_array->x[i];
        tail_y *= vec_array->y[i];
        tail_z *= vec_array->z[i];
    }

    return (vec3f_t){
        x0 / (hmul4(sx) * tail_x),
        y0 / (hmul4(sy) * tail_y),
        z0 / (hmul4(sz) * tail_z)
    };
}

vec3f_arr_t *vec3f_arr_add(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict a, const vec3f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);
        __m128 az = _mm_loadu_ps(&a->z[i]);
        __m128 bz = _mm_loadu_ps(&b->z[i]);

        _mm_storeu_ps(&out->x[i], _mm_add_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_add_ps(ay, by));
        _mm_storeu_ps(&out->z[i], _mm_add_ps(az, bz));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] + b->x[i];
        out->y[i] = a->y[i] + b->y[i];
        out->z[i] = a->z[i] + b->z[i];
    }

    return out;
}

vec3f_arr_t *vec3f_arr_sub(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict a, const vec3f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);
        __m128 az = _mm_loadu_ps(&a->z[i]);
        __m128 bz = _mm_loadu_ps(&b->z[i]);

        _mm_storeu_ps(&out->x[i], _mm_sub_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_sub_ps(ay, by));
        _mm_storeu_ps(&out->z[i], _mm_sub_ps(az, bz));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] - b->x[i];
        out->y[i] = a->y[i] - b->y[i];
        out->z[i] = a->z[i] - b->z[i];
    }

    return out;
}

vec3f_arr_t *vec3f_arr_mul(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict a, const vec3f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);
        __m128 az = _mm_loadu_ps(&a->z[i]);
        __m128 bz = _mm_loadu_ps(&b->z[i]);

        _mm_storeu_ps(&out->x[i], _mm_mul_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_mul_ps(ay, by));
        _mm_storeu_ps(&out->z[i], _mm_mul_ps(az, bz));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] * b->x[i];
        out->y[i] = a->y[i] * b->y[i];
        out->z[i] = a->z[i] * b->z[i];
    }

    return out;
}

vec3f_arr_t *vec3f_arr_div(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict a, const vec3f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);
        __m128 az = _mm_loadu_ps(&a->z[i]);
        __m128 bz = _mm_loadu_ps(&b->z[i]);

        _mm_storeu_ps(&out->x[i], _mm_div_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_div_ps(ay, by));
        _mm_storeu_ps(&out->z[i], _mm_div_ps(az, bz));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] / b->x[i];
        out->y[i] = a->y[i] / b->y[i];
        out->z[i] = a->z[i] / b->z[i];
    }

    return out;
}

vec3f_arr_t *vec3f_arr_scale(vec3f_arr_t *restrict out, const vec3f_arr_t *restrict vec_array, float s, size_t count) {
    const __m128 scale = _mm_set1_ps(s);

    size_t i = 0;

    for (; i + 4 <= count; i += 4) {
        __m128 x = _mm_loadu_ps(&vec_array->x[i]);
        __m128 y = _mm_loadu_ps(&vec_array->y[i]);
        __m128 z = _mm_loadu_ps(&vec_array->z[i]);

        _mm_storeu_ps(
            &out->x[i],
            _mm_mul_ps(x, scale)
        );

        _mm_storeu_ps(
            &out->y[i],
            _mm_mul_ps(y, scale)
        );

        _mm_storeu_ps(
            &out->z[i],
            _mm_mul_ps(z, scale)
        );
    }

    for (; i < count; ++i) {
        out->x[i] = vec_array->x[i] * s;
        out->y[i] = vec_array->y[i] * s;
        out->z[i] = vec_array->z[i] * s;
    }

    return out;
}


// ----------------------------------------------------------------------------------------------------------------------------
// vec4

vec4f_t vec4f_add(const vec4f_t a, const vec4f_t b) {
    return (vec4f_t){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

vec4f_t vec4f_add3(const vec4f_t a, const vec4f_t b, const vec4f_t c) {
    return (vec4f_t){a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z, a.w + b.w + c.w};
}

vec4f_t vec4f_mul(const vec4f_t a, const vec4f_t b) {
    return (vec4f_t){a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

vec4f_t vec4f_mul3(const vec4f_t a, const vec4f_t b, const vec4f_t c) {
    return (vec4f_t){a.x * b.x * c.x, a.y * b.y * c.y, a.z * b.z * c.z, a.w * b.w * c.w};
}

vec4f_t vec4f_div(const vec4f_t a, const vec4f_t b) {
    return (vec4f_t){a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
}

vec4f_t vec4f_div3(const vec4f_t a, const vec4f_t b, const vec4f_t c) {
    return (vec4f_t){a.x / b.x / c.x, a.y / b.y / c.y, a.z / b.z / c.z, a.w / b.w / c.w};
}

vec4f_t vec4f_sub(const vec4f_t a, const vec4f_t b) {
    return (vec4f_t){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

vec4f_t vec4f_sub3(const vec4f_t a, const vec4f_t b, const vec4f_t c) {
    return (vec4f_t){a.x - b.x - c.x, a.y - b.y - c.y, a.z - b.z - c.z, a.w - b.w - c.w};
}

float vec4f_dot(const vec4f_t a, const vec4f_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float vec4f_dot3(const vec4f_t a, const vec4f_t b, const vec4f_t c) {
    return a.x * b.x * c.x + a.y * b.y * c.y + a.z * b.z * c.z + a.w * b.w * c.w;
}

vec4f_t vec4f_n_add(const vec4f_arr_t *vec_array, size_t count) {
    vec4f_t result = {0.0f, 0.0f, 0.0f, 0.0f};

    size_t i = 0;
    __m128 sumx = _mm_setzero_ps();
    __m128 sumy = _mm_setzero_ps();
    __m128 sumz = _mm_setzero_ps();
    __m128 sumw = _mm_setzero_ps();

    for (; i + 4 <= count; i += 4) {
        sumx = _mm_add_ps(sumx, _mm_loadu_ps(&vec_array->x[i]));
        sumy = _mm_add_ps(sumy, _mm_loadu_ps(&vec_array->y[i]));
        sumz = _mm_add_ps(sumz, _mm_loadu_ps(&vec_array->z[i]));
        sumw = _mm_add_ps(sumw, _mm_loadu_ps(&vec_array->w[i]));
    }

    result.x = hsum4_ps(sumx); 
    result.y = hsum4_ps(sumy); 
    result.z = hsum4_ps(sumz); 
    result.w = hsum4_ps(sumw); 

    for (; i < count; ++i) {
        result.x += vec_array->x[i];
        result.y += vec_array->y[i];
        result.z += vec_array->z[i];
        result.w += vec_array->w[i];
    }

    return result;
}

vec4f_t vec4f_n_sub(const vec4f_arr_t *vec_array, size_t count) {
    const float x0 = vec_array->x[0];
    const float y0 = vec_array->y[0];
    const float z0 = vec_array->z[0];
    const float w0 = vec_array->w[0];

    __m128 sx = _mm_setzero_ps();
    __m128 sy = _mm_setzero_ps();
    __m128 sz = _mm_setzero_ps();
    __m128 sw = _mm_setzero_ps();

    size_t i = 1;

    for (; i + 4 <= count; i += 4) {
        sx = _mm_add_ps(sx, _mm_loadu_ps(vec_array->x + i));
        sy = _mm_add_ps(sy, _mm_loadu_ps(vec_array->y + i));
        sz = _mm_add_ps(sz, _mm_loadu_ps(vec_array->z + i));
        sw = _mm_add_ps(sw, _mm_loadu_ps(vec_array->w + i));
    }

    float tail_x = 0.0f;
    float tail_y = 0.0f;
    float tail_z = 0.0f;
    float tail_w = 0.0f;

    for (; i < count; ++i) {
        tail_x += vec_array->x[i];
        tail_y += vec_array->y[i];
        tail_z += vec_array->z[i];
        tail_w += vec_array->w[i];
    }

    return (vec4f_t){
        x0 - (hsum4_ps(sx) + tail_x),
        y0 - (hsum4_ps(sy) + tail_y),
        z0 - (hsum4_ps(sz) + tail_z),
        w0 - (hsum4_ps(sw) + tail_w)
    }; 
}

vec4f_t vec4f_n_mul(const vec4f_arr_t *vec_array, size_t count) {
    vec4f_t result = {1.0f, 1.0f, 1.0f, 1.0f};

    size_t i = 0;
    __m128 prodx = _mm_set1_ps(1.0f);
    __m128 prody = _mm_set1_ps(1.0f);
    __m128 prodz = _mm_set1_ps(1.0f);
    __m128 prodw = _mm_set1_ps(1.0f);

    for (; i + 4 <= count; i += 4) {
        prodx = _mm_mul_ps(prodx, _mm_loadu_ps(&vec_array->x[i]));
        prody = _mm_mul_ps(prody, _mm_loadu_ps(&vec_array->y[i]));
        prodz = _mm_mul_ps(prodz, _mm_loadu_ps(&vec_array->z[i]));
        prodw = _mm_mul_ps(prodw, _mm_loadu_ps(&vec_array->w[i]));
    }

    result.x = hsum4_ps(prodx); 
    result.y = hsum4_ps(prody); 
    result.z = hsum4_ps(prodz); 
    result.w = hsum4_ps(prodw); 

    for (; i < count; ++i) {
        result.x *= vec_array->x[i];
        result.y *= vec_array->y[i];
        result.z *= vec_array->z[i];
        result.w *= vec_array->w[i];
    }

    return result;
}

vec4f_t vec4f_n_div(const vec4f_arr_t *restrict vec_array, size_t count) {
    const float x0 = vec_array->x[0];
    const float y0 = vec_array->y[0];
    const float z0 = vec_array->z[0];
    const float w0 = vec_array->w[0];

    __m128 sx = _mm_set1_ps(1.0f);
    __m128 sy = _mm_set1_ps(1.0f);
    __m128 sz = _mm_set1_ps(1.0f);
    __m128 sw = _mm_set1_ps(1.0f);

    size_t i = 1;

    for (; i + 4 <= count; i += 4) {
        sx = _mm_mul_ps(sx, _mm_loadu_ps(vec_array->x + i));
        sy = _mm_mul_ps(sy, _mm_loadu_ps(vec_array->y + i));
        sz = _mm_mul_ps(sz, _mm_loadu_ps(vec_array->z + i));
        sw = _mm_mul_ps(sw, _mm_loadu_ps(vec_array->w + i));
    }

    float tail_x = 1.0f;
    float tail_y = 1.0f;
    float tail_z = 1.0f;
    float tail_w = 1.0f;

    for (; i < count; ++i) {
        tail_x *= vec_array->x[i];
        tail_y *= vec_array->y[i];
        tail_z *= vec_array->z[i];
        tail_w *= vec_array->w[i];
    }

    return (vec4f_t){
        x0 / (hmul4(sx) * tail_x),
        y0 / (hmul4(sy) * tail_y),
        z0 / (hmul4(sz) * tail_z),
        w0 / (hmul4(sw) * tail_w)
    };
}

vec4f_arr_t *vec4f_arr_add(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict a, const vec4f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);
        __m128 az = _mm_loadu_ps(&a->z[i]);
        __m128 bz = _mm_loadu_ps(&b->z[i]);
        __m128 aw = _mm_loadu_ps(&a->w[i]);
        __m128 bw = _mm_loadu_ps(&b->w[i]);

        _mm_storeu_ps(&out->x[i], _mm_add_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_add_ps(ay, by));
        _mm_storeu_ps(&out->z[i], _mm_add_ps(az, bz));
        _mm_storeu_ps(&out->w[i], _mm_add_ps(aw, bw));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] + b->x[i];
        out->y[i] = a->y[i] + b->y[i];
        out->z[i] = a->z[i] + b->z[i];
        out->w[i] = a->w[i] + b->w[i];
    }

    return out;
}

vec4f_arr_t *vec4f_arr_sub(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict a, const vec4f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);
        __m128 az = _mm_loadu_ps(&a->z[i]);
        __m128 bz = _mm_loadu_ps(&b->z[i]);
        __m128 aw = _mm_loadu_ps(&a->w[i]);
        __m128 bw = _mm_loadu_ps(&b->w[i]);

        _mm_storeu_ps(&out->x[i], _mm_sub_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_sub_ps(ay, by));
        _mm_storeu_ps(&out->z[i], _mm_sub_ps(az, bz));
        _mm_storeu_ps(&out->w[i], _mm_sub_ps(aw, bw));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] - b->x[i];
        out->y[i] = a->y[i] - b->y[i];
        out->z[i] = a->z[i] - b->z[i];
        out->w[i] = a->w[i] - b->w[i];
    }

    return out;
}

vec4f_arr_t *vec4f_arr_mul(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict a, const vec4f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);
        __m128 az = _mm_loadu_ps(&a->z[i]);
        __m128 bz = _mm_loadu_ps(&b->z[i]);
        __m128 aw = _mm_loadu_ps(&a->w[i]);
        __m128 bw = _mm_loadu_ps(&b->w[i]);

        _mm_storeu_ps(&out->x[i], _mm_mul_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_mul_ps(ay, by));
        _mm_storeu_ps(&out->z[i], _mm_mul_ps(az, bz));
        _mm_storeu_ps(&out->w[i], _mm_mul_ps(aw, bw));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] * b->x[i];
        out->y[i] = a->y[i] * b->y[i];
        out->z[i] = a->z[i] * b->z[i];
        out->w[i] = a->w[i] * b->w[i];
    }

    return out;
}

vec4f_arr_t *vec4f_arr_div(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict a, const vec4f_arr_t *restrict b, size_t count) {
    size_t i = 0;
    for (; i + 4 <= count; i += 4) {
        __m128 ax = _mm_loadu_ps(&a->x[i]);
        __m128 bx = _mm_loadu_ps(&b->x[i]);
        __m128 ay = _mm_loadu_ps(&a->y[i]);
        __m128 by = _mm_loadu_ps(&b->y[i]);
        __m128 az = _mm_loadu_ps(&a->z[i]);
        __m128 bz = _mm_loadu_ps(&b->z[i]);
        __m128 aw = _mm_loadu_ps(&a->w[i]);
        __m128 bw = _mm_loadu_ps(&b->w[i]);

        _mm_storeu_ps(&out->x[i], _mm_div_ps(ax, bx));
        _mm_storeu_ps(&out->y[i], _mm_div_ps(ay, by));
        _mm_storeu_ps(&out->z[i], _mm_div_ps(az, bz));
        _mm_storeu_ps(&out->w[i], _mm_div_ps(aw, bw));
    }

    for (; i < count; ++i) {
        out->x[i] = a->x[i] / b->x[i];
        out->y[i] = a->y[i] / b->y[i];
        out->z[i] = a->z[i] / b->z[i];
        out->w[i] = a->w[i] / b->w[i];
    }

    return out;
}

vec4f_arr_t *vec4f_arr_scale(vec4f_arr_t *restrict out, const vec4f_arr_t *restrict vec_array, float s, size_t count) {
    const __m128 scale = _mm_set1_ps(s);

    size_t i = 0;

    for (; i + 4 <= count; i += 4) {
        __m128 x = _mm_loadu_ps(&vec_array->x[i]);
        __m128 y = _mm_loadu_ps(&vec_array->y[i]);
        __m128 z = _mm_loadu_ps(&vec_array->z[i]);
        __m128 w = _mm_loadu_ps(&vec_array->w[i]);

        _mm_storeu_ps(
            &out->x[i],
            _mm_mul_ps(x, scale)
        );

        _mm_storeu_ps(
            &out->y[i],
            _mm_mul_ps(y, scale)
        );

        _mm_storeu_ps(
            &out->z[i],
            _mm_mul_ps(z, scale)
        );

        _mm_storeu_ps(
            &out->w[i],
            _mm_mul_ps(w, scale) 
        );
    }

    for (; i < count; ++i) {
        out->x[i] = vec_array->x[i] * s;
        out->y[i] = vec_array->y[i] * s;
        out->z[i] = vec_array->z[i] * s;
        out->w[i] = vec_array->w[i] * s;
    }

    return out;
}