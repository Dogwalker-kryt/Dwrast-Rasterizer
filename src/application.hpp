#pragma once

#include "rasterizer/dwrast_rasterizer.hpp"
#include <gtk/gtk.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>

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
    if (state->frame_buffer_) {
        dwrast::destroy_FB2(state->frame_buffer_);
        state->frame_buffer_ = nullptr;
    }
}


inline void draw_random_primitives(
    dwrast::FB2 *fb,
    const uint32_t primitive_count = 128,
    const uint32_t seed = std::random_device{}()) {
    if (!fb || !fb->buffer || fb->width == 0 || fb->heigth == 0) return;

    // Keep generated coordinates representable by the signed raster types.
    const uint32_t drawable_width = std::min(fb->width, static_cast<uint32_t>(INT32_MAX / 2));
    const uint32_t drawable_height = std::min(fb->heigth, static_cast<uint32_t>(INT32_MAX / 2));
    const int32_t min_x = -static_cast<int32_t>(drawable_width / 4);
    const int32_t min_y = -static_cast<int32_t>(drawable_height / 4);
    const int32_t max_x = static_cast<int32_t>(drawable_width + drawable_width / 4);
    const int32_t max_y = static_cast<int32_t>(drawable_height + drawable_height / 4);

    std::mt19937 rng(seed);
    std::uniform_int_distribution<int32_t> x_dist(min_x, max_x);
    std::uniform_int_distribution<int32_t> y_dist(min_y, max_y);
    std::uniform_int_distribution<uint32_t> channel_dist(0, 255);
    std::uniform_int_distribution<uint32_t> primitive_dist(0, 1);

    const auto random_color = [&]() {
        const uint32_t red = channel_dist(rng);
        const uint32_t green = channel_dist(rng);
        const uint32_t blue = channel_dist(rng);
        return 0xFF000000u |
            (red << 16) |
            (green << 8) |
            blue;
    };
    const auto random_point = [&]() {
        return rpixel_t{x_dist(rng), y_dist(rng)};
    };

    dwrast::clear_buf_FB2(fb, BLACK);

    for (uint32_t i = 0; i < primitive_count; ++i) {
        const color_t color = random_color();
        if (primitive_dist(rng) == 0) {
            dwrast::draw_line(fb, random_point(), random_point(), color);
            continue;
        }

        const rpixel_t p0 = random_point();
        const rpixel_t p1 = random_point();
        const rpixel_t p2 = random_point();
        triangle_t triangle = {
            {{static_cast<float>(p0.x), static_cast<float>(p0.y), 0.0f, 1.0f}, color},
            {{static_cast<float>(p1.x), static_cast<float>(p1.y), 0.0f, 1.0f}, color},
            {{static_cast<float>(p2.x), static_cast<float>(p2.y), 0.0f, 1.0f}, color}
        };

        dwrast::draw_triangle(fb, &triangle, color, random_color());
    }
}

inline void draw_triangles(dwrast::FB2 *fb) {
    // The final framebuffer is consumed by PPM export and/or GTK, so the
    // randomized rasterization writes are observable and remain meaningful.
    draw_random_primitives(fb);
}

inline void render_frame(application_t *state) {
    draw_triangles(state->frame_buffer_);
    gtk_widget_queue_draw(state->window_);
}