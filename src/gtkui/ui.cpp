#include "ui.hpp"

void draw(GtkDrawingArea* area, cairo_t* cr, int width, int height, gpointer data) {
    application_t *state = static_cast<application_t*>(data);

    cairo_surface_t* surface = cairo_image_surface_create_for_data(
        reinterpret_cast<unsigned char*>(state->frame_buffer_->data()),
        CAIRO_FORMAT_ARGB32,
        state->frame_buffer_->width(),
        state->frame_buffer_->height(),
        state->frame_buffer_->width() * sizeof(uint32_t)
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

void activate(GtkApplication *app, gpointer data) {
    application_t *state = static_cast<application_t*>(data);

    state->window_ = gtk_application_window_new(state->gtk_app_);


    gtk_window_set_title(GTK_WINDOW(state->window_), "CPU Rasterizer");

    state->area_ = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(state->area_), 1000);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(state->area_), 500);

    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(state->area_), draw, &state->frame_buffer_ ,nullptr);

    gtk_window_set_child(GTK_WINDOW(state->window_), state->area_);
    gtk_window_present(GTK_WINDOW(state->window_));

    render_frame(state);
}