#pragma once

#include "rasterizer/cpu_rasterizer.hpp"
#include <gtk/gtk.h>

struct application_t {
    FB *frame_buffer_ = nullptr;

    GtkApplication *gtk_app_ = nullptr;
    GtkWidget *window_ = nullptr;
    GtkWidget *area_ = nullptr;

    bool use_gtk_ = false;

    int argc_{0};
    char **argv_;

    int8_t exit_code{1};
};

inline void free_state(application_t *state) {
    delete state->frame_buffer_;
}

inline void render_frame(application_t *state) {
    state->frame_buffer_->clear(BLACK);

    // here comes my rendere stuff eventualy


    gtk_widget_queue_draw(state->window_);
}