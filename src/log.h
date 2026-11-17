#pragma once

/**
 * Terminal output helpers, usable from C and C++.
 *
 * every line looks like:  [tag]   message
 * the ANSI_* macros are string literals so they can be glued into format strings:
 *     log_info("loaded " ANSI_BOLD "%s" ANSI_RESET, path);
 *
 * colors are stripped automatically when stdout is not a terminal or NO_COLOR is set
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ANSI_RESET   "\033[0m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_DIM     "\033[2m"
#define ANSI_GRAY    "\033[90m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"

#define LOG_LINE_MAX 1024

static inline bool log_use_color(void) {
    static int use_color = -1;
    if (use_color < 0) use_color = isatty(STDOUT_FILENO) && getenv("NO_COLOR") == NULL;
    return use_color != 0;
}

/**
 * @brief removes every "ESC [ ... m" sequence in place
 */
static inline void log_strip_ansi(char *text) {
    char *out = text;

    for (const char *in = text; *in; ++in) {
        if (in[0] == '\033' && in[1] == '[') {
            in += 2;
            while (*in && *in != 'm') ++in;
            if (!*in) break;
            continue;
        }
        *out++ = *in;
    }
    *out = '\0';
}

/**
 * @param tag_color color of the [tag]
 * @param text_color color for the whole message, "" for the default terminal color
 */
static inline void log_vwrite(const char *tag_color, const char *tag, const char *text_color, const char *fmt, va_list args) {
    char message[LOG_LINE_MAX];
    vsnprintf(message, sizeof(message), fmt, args);

    char line[LOG_LINE_MAX + 64];
    snprintf(line, sizeof(line), "%s%-8s" ANSI_RESET " %s%s" ANSI_RESET "\n", tag_color, tag, text_color, message);

    if (!log_use_color()) log_strip_ansi(line);
    fputs(line, stdout);
}

#define LOG_DEFINE(name, tag_color, tag, text_color) \
    __attribute__((format(printf, 1, 2))) \
    static inline void name(const char *fmt, ...) { \
        va_list args; \
        va_start(args, fmt); \
        log_vwrite(tag_color, tag, text_color, fmt, args); \
        va_end(args); \
    }

LOG_DEFINE(log_info,  ANSI_BOLD ANSI_BLUE,    "[info]",  "")
LOG_DEFINE(log_ok,    ANSI_BOLD ANSI_GREEN,   "[ok]",    "")
LOG_DEFINE(log_warn,  ANSI_BOLD ANSI_YELLOW,  "[warn]",  "")
LOG_DEFINE(log_error, ANSI_BOLD ANSI_RED,     "[error]", "")
LOG_DEFINE(log_bench, ANSI_BOLD ANSI_MAGENTA, "[bench]", "")
LOG_DEFINE(log_gtk,   ANSI_BOLD ANSI_CYAN,    "[gtk]",   "")
LOG_DEFINE(log_debug, ANSI_GRAY,              "[debug]", ANSI_GRAY)
LOG_DEFINE(log_exit,  ANSI_GRAY,              "[exit]",  ANSI_GRAY)

#undef LOG_DEFINE

/**
 * @brief prints the "DwRast  version · cpu rasterizer" header
 */
static inline void log_header(const char *version) {
    char line[256];
    snprintf(line, sizeof(line), "\n  " ANSI_BOLD ANSI_MAGENTA "DwRast" ANSI_RESET "  " ANSI_GRAY "v%s · cpu rasterizer" ANSI_RESET "\n\n", version);

    if (!log_use_color()) log_strip_ansi(line);
    fputs(line, stdout);
}
