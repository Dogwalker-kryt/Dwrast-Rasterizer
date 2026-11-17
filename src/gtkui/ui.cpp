#include "ui.hpp"
#include "../log.h"
#include <locale.h>
#include <glib-unix.h>
#include <signal.h>

/**
 * @brief ctrl+c / SIGTERM in the terminal closes the app normally so state gets freed and stats get printed
 */
static gboolean quit_on_signal(gpointer data) {
    g_application_quit(G_APPLICATION(data));
    return G_SOURCE_REMOVE;
}

void print_realtime_stats(const application_t *state) {
    if (state->realtime_frames == 0) return;

    const double seconds = static_cast<double>(state->last_frame_time_us - state->realtime_start_us) / 1.0e6;
    const double avg_raster_ms = state->realtime_raster_ms / state->realtime_frames;

    log_gtk(ANSI_BOLD "%lu" ANSI_RESET " frames in " ANSI_BOLD "%.1f s" ANSI_RESET ANSI_GRAY "  →  " ANSI_RESET
            ANSI_BOLD ANSI_GREEN "%.1f fps" ANSI_RESET ANSI_GRAY " shown" ANSI_RESET,
            state->realtime_frames, seconds, seconds > 0.0 ? (state->realtime_frames - 1) / seconds : 0.0);
    log_gtk("avg raster " ANSI_BOLD "%.3f ms" ANSI_RESET ANSI_GRAY " (%.0f fps max) · " ANSI_RESET
            ANSI_BOLD "%zu" ANSI_RESET ANSI_GRAY " triangles" ANSI_RESET,
            avg_raster_ms, 1000.0 / avg_raster_ms, state->triangle_count);
}

void draw(GtkDrawingArea* area, cairo_t* cr, int width, int height, gpointer data) {
    (void)area;
    (void)width;
    (void)height;
    application_t *state = static_cast<application_t*>(data);

    cairo_surface_t* surface = cairo_image_surface_create_for_data(
        reinterpret_cast<unsigned char*>(state->frame_buffer_->buffer),
        CAIRO_FORMAT_ARGB32,
        static_cast<int>(state->frame_buffer_->width),
        static_cast<int>(state->frame_buffer_->heigth),
        static_cast<int>(state->frame_buffer_->width * sizeof(uint32_t))
    );

    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    cairo_surface_destroy(surface);
}

// void activate_test(GtkApplication* app, gpointer) {
//     static cpu_rast::Framebuffer<1000, 500> frame_buffer;
//     frame_buffer.clear(BLACK);

//     // Test some pixels.
//     for (int y = 100; y < 300; ++y) {
//         for (int x = 100; x < 400; ++x) {
//             frame_buffer.set_pixel(x, y, GREEN);
//         }
//     }

//     GtkWidget* window = gtk_application_window_new(app);

//     gtk_window_set_title(GTK_WINDOW(window), "CPU Rasterizer");

//     GtkWidget* area = gtk_drawing_area_new();

//     gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area), 1000);

//     gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area), 500);

//     gtk_drawing_area_set_draw_func(
//         GTK_DRAWING_AREA(area),
//         draw,
//         &frame_buffer,
//         nullptr
//     );

//     gtk_window_set_child(GTK_WINDOW(window), area);

//     gtk_window_present(GTK_WINDOW(window));
// }

/**
 * @brief runs once per monitor refresh, spins the camera, rasterizes a new frame and shows fps in the title
 */
static gboolean realtime_tick(GtkWidget *widget, GdkFrameClock *clock, gpointer data) {
    application_t *state = static_cast<application_t*>(data);
    const int64_t now_us = gdk_frame_clock_get_frame_time(clock);

    if (state->last_frame_time_us == 0) {
        state->last_frame_time_us = now_us;
        state->stats_window_start_us = now_us;
        state->realtime_start_us = now_us;
    }

    constexpr float spin_speed = 0.8f; // rad/s
    constexpr float two_pi = 6.28318530718f;
    const float dt = static_cast<float>(now_us - state->last_frame_time_us) / 1.0e6f;
    state->camera.yaw = fmodf(state->camera.yaw + spin_speed * dt, two_pi);

    const double raster_ms = rasterize_frame_timed(state);
    state->stats_raster_ms += raster_ms;
    state->stats_frames++;
    state->realtime_raster_ms += raster_ms;
    state->realtime_frames++;
    state->last_frame_time_us = now_us;
    gtk_widget_queue_draw(widget);

    const int64_t window_us = now_us - state->stats_window_start_us;
    if (window_us >= 500000 && state->stats_frames > 0) {
        const double fps = state->stats_frames / (static_cast<double>(window_us) / 1.0e6);

        const double avg_raster_ms = state->stats_raster_ms / state->stats_frames;

        char title[192];
        snprintf(title, sizeof(title), "DwRast Rasterizer  |  %.0f fps  |  raster %.2f ms (%.0f fps max)  |  %zu tris",
                 fps, avg_raster_ms, 1000.0 / avg_raster_ms, state->triangle_count);
        gtk_window_set_title(GTK_WINDOW(state->window_), title);

        state->stats_window_start_us = now_us;
        state->stats_frames = 0;
        state->stats_raster_ms = 0.0;
    }

    return G_SOURCE_CONTINUE;
}

void activate(GtkApplication *app, gpointer data) {
    (void)app;
    application_t *state = static_cast<application_t*>(data);

    // gtk switched to the system locale, keep numbers as "1.5" instead of "1,5" in the title and logs
    setlocale(LC_NUMERIC, "C");

    state->window_ = gtk_application_window_new(state->gtk_app_);


    gtk_window_set_title(GTK_WINDOW(state->window_), "DwRast Rasterizer");

    state->area_ = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(state->area_), state->width);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(state->area_), state->heigth);

    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(state->area_), draw, state ,nullptr);

    gtk_window_set_child(GTK_WINDOW(state->window_), state->area_);
    gtk_window_present(GTK_WINDOW(state->window_));

    render_frame_gtk(state);

    // a loaded model spins in real time, the demo triangles are static
    if (state->has_model) gtk_widget_add_tick_callback(state->area_, realtime_tick, state, nullptr);

    g_unix_signal_add(SIGINT, quit_on_signal, state->gtk_app_);
    g_unix_signal_add(SIGTERM, quit_on_signal, state->gtk_app_);
}