#pragma once

#include "rasterizer/dwrast_rasterizer.hpp"
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


inline void draw_triangles(dwrast::FB2 *fb) {
    dwrast::clear_buf_FB2(fb, BLACK);

    vertex_t v0 = {
        {250.0f, 80.0f, 0.0f, 1.0f},
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

    triangle_t triangle = create_triangle_from_vertex(v0, v1, v2);
    dwrast::draw_triangle(fb, &triangle, BLUE, BLACK);
}

inline void render_frame(application_t *state) {
    draw_triangles(state->frame_buffer_);
    gtk_widget_queue_draw(state->window_);
}