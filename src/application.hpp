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
    dwrast::DB *depth_buffer = nullptr;

    GtkApplication *gtk_app_ = nullptr;
    GtkWidget *window_ = nullptr;
    GtkWidget *area_ = nullptr;

    bool use_gtk_ = false;
    bool write_ppm = false;
    char ppm_file_name[64];
    bool use_depth_buffer = false;
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
    vertex_t v0 = {
        {600.0f, 180.0f, 0.0f, 1.0f},
        WHITE
    };

    vertex_t v0_1 = {
        {500.0f, 180.0f, 0.0f, 1.0f},
        WHITE
    };

    vertex_t v1 = {
        {500.0f, 260.0f, 0.0f, 1.0f},
        WHITE
    };

    vertex_t v2 = {
        {600.0f, 260.0f, 0.0f, 1.0f},
        WHITE
    };

    vertex_t master_point = {
        {550.0f, 100.0f, 0.0f, 1.0f},
        WHITE
    };

    rpixel_t v0_rp = { v0.pos.x, v0.pos.y };
    rpixel_t v01_rp = { v0_1.pos.x, v0_1.pos.y };
    rpixel_t v1_rp = { v1.pos.x, v1.pos.y };
    rpixel_t v2_rp = { v2.pos.x, v2.pos.y };
    rpixel_t master_point_rp = { master_point.pos.x, master_point.pos.y };

    triangle_t tri1 = { master_point, v1, v0_1 };
    triangle_t tri2 = { master_point, v1, v2 };
    triangle_t tri3 = { master_point, v0_1, v0 };
    triangle_t tri4 = { master_point, v2, v0 };

    dwrast::fill_triangle_avx2(fb, &tri3, RED);
    dwrast::fill_triangle_avx2(fb, &tri1, WHITE);
    dwrast::fill_triangle_avx2(fb, &tri2, GREEN);
    dwrast::fill_triangle_avx2(fb, &tri4, BLUE);
}

inline void *rasterize_frame(application_t *state) {
    
}

inline void render_frame_gtk(application_t *state) {
    rasterize_frame(state);
    gtk_widget_queue_draw(state->window_);
}