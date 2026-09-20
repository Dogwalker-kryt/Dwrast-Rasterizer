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

using color_t = uint32_t;


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