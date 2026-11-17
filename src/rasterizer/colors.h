#pragma once

#include <stdint.h>

#define WHITE 0xFFFFFFFF
#define BLACK 0xFF000000
#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF
#define YELLOW 0xFFFFFF00
#define CYAN 0xFF00FFFF
#define MAGENTA 0xFFFF00FF
#define ORANGE 0xFFFFA500
#define PURPLE 0xFF800080
#define LIGHT_GRAY 0xFFD3D3D3
#define DARK_GRAY 0xFFA9A9A9
#define TRANSPARENT 0x00000000

#define COLOR(r, g, b) (0xFF000000 | ((r) << 16) | ((g) << 8) | (b))

typedef uint32_t color_t;
typedef color_t argb_t;

/**
 * @brief multiplies r, g and b by intensity (clamped to [0, 1]), alpha stays the same
 */
static inline color_t color_scale(const color_t color, float intensity) {
    intensity = intensity < 0.0f ? 0.0f : (intensity > 1.0f ? 1.0f : intensity);

    const uint32_t r = (uint32_t)((float)((color >> 16) & 0xFF) * intensity + 0.5f);
    const uint32_t g = (uint32_t)((float)((color >> 8) & 0xFF) * intensity + 0.5f);
    const uint32_t b = (uint32_t)((float)(color & 0xFF) * intensity + 0.5f);

    return (color & 0xFF000000) | (r << 16) | (g << 8) | b;
}

static inline const char *reset()   { return "\033[0m"; }
static inline const char *red()     { return "\033[31m"; }
static inline const char *green()   { return "\033[32m"; }
static inline const char *yellow()  { return "\033[33m"; }
static inline const char *blue()    { return "\033[34m"; }
static inline const char *magenta() { return "\033[35m"; }
static inline const char *cyan()    { return "\033[36m"; }
static inline const char *bold()    { return "\033[1m"; }
static inline const char *inverse() { return "\033[7m"; }

#define RESET_ANSI   reset()
#define RED_ANSI     red()
#define GREEN_ANSI   green()
#define YELLOW_ANSI  yellow()
#define BLUE_ANSI    blue()
#define MAGENTA_ANSI magenta()
#define CYAN_ANSI    cyan()
#define BOLD_ANSI    bold()
#define INVERSE_ANSI inverse()