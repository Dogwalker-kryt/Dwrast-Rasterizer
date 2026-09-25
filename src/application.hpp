#pragma once

#include "rasterizer/cpu_rasterizer.hpp"
#include <gtk/gtk.h>

struct application_t {
    dwrast::FB2 *frame_buffer_ = nullptr;

    GtkApplication *gtk_app_ = nullptr;
    GtkWidget *window_ = nullptr;
    GtkWidget *area_ = nullptr;

    bool use_gtk_ = false;
    bool write_ppm = false;
    char file_name[64];
    bool use_custom_FB_args;
    uint64_t width = WIDTH;
    uint64_t heigth = HEIGTH;

    int argc_{0};
    char **argv_;

    int8_t exit_code{0};
};

inline void free_state(application_t *state) {
    delete state->frame_buffer_;
}

// #include <algorithm>
// #include <cstdint>

// struct Vec2i {
//     int x;
//     int y;
// };

// static inline void draw_line(FB* fb, Vec2i a, Vec2i b, uint32_t color)
// {
//     int x0 = a.x;
//     int y0 = a.y;
//     int x1 = b.x;
//     int y1 = b.y;

//     const int dx = std::abs(x1 - x0);
//     const int sx = (x0 < x1) ? 1 : -1;
//     const int dy = -std::abs(y1 - y0);
//     const int sy = (y0 < y1) ? 1 : -1;

//     int error = dx + dy;

//     for (;;) {
//         fb->set_pixel(x0, y0, color);

//         if (x0 == x1 && y0 == y1)
//             break;

//         const int e2 = 2 * error;

//         if (e2 >= dy) {
//             error += dy;
//             x0 += sx;
//         }

//         if (e2 <= dx) {
//             error += dx;
//             y0 += sy;
//         }
//     }
// }

// static inline int64_t edge_function(Vec2i a, Vec2i b, int x, int y)
// {
//     return static_cast<int64_t>(x - a.x) * (b.y - a.y) -
//            static_cast<int64_t>(y - a.y) * (b.x - a.x);
// }

// static inline void fill_triangle(
//     FB* fb,
//     Vec2i v0,
//     Vec2i v1,
//     Vec2i v2,
//     uint32_t color)
// {
//     int min_x = std::min({v0.x, v1.x, v2.x});
//     int max_x = std::max({v0.x, v1.x, v2.x});
//     int min_y = std::min({v0.y, v1.y, v2.y});
//     int max_y = std::max({v0.y, v1.y, v2.y});

//     const int64_t area =
//         edge_function(v0, v1, v2.x, v2.y);

//     if (area == 0)
//         return;

//     for (int y = min_y; y <= max_y; ++y) {
//         for (int x = min_x; x <= max_x; ++x) {
//             const int64_t e0 =
//                 edge_function(v0, v1, x, y);

//             const int64_t e1 =
//                 edge_function(v1, v2, x, y);

//             const int64_t e2 =
//                 edge_function(v2, v0, x, y);

//             const bool inside =
//                 (e0 >= 0 && e1 >= 0 && e2 >= 0) ||
//                 (e0 <= 0 && e1 <= 0 && e2 <= 0);

//             if (inside)
//                 fb->set_pixel(x, y, color);
//         }
//     }
// }

// static inline void draw_triangle(
//     FB* fb,
//     Vec2i v0,
//     Vec2i v1,
//     Vec2i v2,
//     uint32_t fill_color,
//     uint32_t outline_color)
// {
//     fill_triangle(fb, v0, v1, v2, fill_color);

//     draw_line(fb, v0, v1, outline_color);
//     draw_line(fb, v1, v2, outline_color);
//     draw_line(fb, v2, v0, outline_color);
// }

// inline void draw_triangles(FB* fb)
// {
//     fb->clear(0xFF000000); // AARRGGBB

//     // Red triangle.
//     draw_triangle(
//         fb,
//         {100, 100},
//         {200, 100},
//         {150, 200},
//         0xFFFF0000, // Fill
//         0xFFFFFFFF  // Outline
//     );

//     // Green triangle.
//     draw_triangle(
//         fb,
//         {500, 100},
//         {600, 100},
//         {550, 200},
//         0xFF00FF00,
//         0xFFFFFFFF
//     );

//     // Blue triangle.
//     draw_triangle(
//         fb,
//         {300, 400},
//         {400, 400},
//         {350, 500},
//         0xFF0000FF,
//         0xFFFFFFFF
//     );
// }


inline void render_frame(application_t *state) {
    dwrast::clear_buf_FB2(state->frame_buffer_, BLACK);

    // here comes my rendere stuff eventualy
    // draw_triangles(state->frame_buffer_);

    gtk_widget_queue_draw(state->window_);
}