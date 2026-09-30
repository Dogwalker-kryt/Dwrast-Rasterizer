#pragma once

#include "rasterizer/dwrast_rasterizer.hpp"
#include <gtk/gtk.h>
#include <chrono>
#include <iostream>

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


inline void draw_triangles(dwrast::FB2 *fb) {
    dwrast::clear_buf_FB2(fb, BLACK);

    vertex_t v0 = {
        {50.0f, 80.0f, 0.0f, 1.0f},
        WHITE
    };

    vertex_t v1 = {
        {100.0f, 420.0f, 0.0f, 1.0f},
        WHITE
    };

    vertex_t v2 = {
        {500.0f, 420.0f, 0.0f, 1.0f},
        WHITE
    };

    triangle_t triangle = { v0, v1, v2 };

    // auto start = std::chrono::steady_clock::now();

    // for (int i = 0; i < 1000; ++i) {
        dwrast::draw_triangle(fb, &triangle, BLUE, BLACK);
    // }

    // auto end = std::chrono::steady_clock::now();

    // double seconds = std::chrono::duration<double>(end - start).count();

    // std::cout << "Time: " << seconds << " s\n";
}

inline void render_frame(application_t *state) {
    draw_triangles(state->frame_buffer_);
    gtk_widget_queue_draw(state->window_);
}