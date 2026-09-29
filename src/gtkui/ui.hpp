#pragma once

#include "../rasterizer/dwrast_rasterizer.hpp"
#include "../application.hpp"
#include <gtk/gtk.h>
#include <cairo.h>


void draw(GtkDrawingArea* area, cairo_t* cr, int width, int height, gpointer data);

// void activate_test(GtkApplication* app, gpointer);
inline void draw_triangle(dwrast::FB2 *fb, const triangle_t *triangle, const color_t fill_color, const color_t outline_color);

void activate(GtkApplication *app, gpointer);
