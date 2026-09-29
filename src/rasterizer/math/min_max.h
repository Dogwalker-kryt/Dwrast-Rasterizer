#pragma once

#include <stdint.h>

__always_inline int32_t min_i32_inline(const int32_t a, const int32_t b) {
    return (a > b) ? b : a;
}

__always_inline int32_t max_i32_inline(const int32_t a, const int32_t b) {
    return (a < b) ? b : a; 
}

__always_inline uint32_t min_u32_inline(const uint32_t a, const uint32_t b) {
    return (a > b) ? b : a;
}

__always_inline uint32_t max_u32_inline(const uint32_t a, const uint32_t b) {
    return (a < b) ? b : a; 
}

__always_inline uint64_t min_u64_inline(const uint64_t a, const uint64_t b) {
    return (a > b) ? b : a;
}

__always_inline uint64_t max_u64_inline(const uint64_t a, const uint64_t b) {
    return (a < b) ? b : a;
}

__always_inline int64_t min_i64_inline(const int64_t a, const int64_t b) {
    return (a > b) ? b : a;
}

__always_inline int64_t max_i64_inline(const int64_t a, const int64_t b) {
    return (a < b) ? b : a;
}

__always_inline int16_t min_i16_inline(const int16_t a, const int16_t b) {
    return (a > b) ? b : a;
}

__always_inline int16_t max_i16_inline(const int16_t a, const int16_t b) {
    return (a < b) ? b : a;
}

__always_inline uint16_t min_u16_inline(const uint16_t a, const uint16_t b) {
    return (a > b) ? b : a;
}

__always_inline uint16_t max_u16_inline(const uint16_t a, const uint16_t b) {
    return (a < b) ? b : a;
}

__always_inline int8_t min_i8_inline(const int8_t a, const int8_t b) {
    return (a > b) ? b : a;
}

__always_inline int8_t max_i8_inline(const int8_t a, const int8_t b) {
    return (a < b) ? b : a;
}

__always_inline uint8_t min_u8_inline(const uint8_t a, const uint8_t b) {
    return (a > b) ? b : a;
}

__always_inline uint8_t max_u8_inline(const uint8_t a, const uint8_t b) {
    return (a < b) ? b : a;
}

__always_inline float min_f_inline(const float a, const float b) {
    return (a > b) ? b : a;
}

__always_inline float max_f_inline(const float a, const float b) {
    return (a < b) ? b : a;
}