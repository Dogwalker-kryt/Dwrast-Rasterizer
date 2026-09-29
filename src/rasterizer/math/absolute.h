#pragma once

#include <stdint.h>

__always_inline int8_t absolute_i8(int8_t x) {
    return (x < 0 ? -x : x);   
}

__always_inline int16_t absolute_i16(int16_t x) {
    return (x < 0 ? -x : x);   
}

__always_inline int32_t absolute_i32(int32_t x) {
    return (x < 0 ? -x : x);   
}

__always_inline int64_t absolute_i64(int64_t x) {
    return (x < 0 ? -x : x);   
}

__always_inline float absolute_f(float x) {
    return (x < 0.0f ? -x : x);   
}