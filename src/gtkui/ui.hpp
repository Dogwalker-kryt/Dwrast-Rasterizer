#pragma once

#include "../rasterizer/cpu_rasterizer.hpp"
#include "../application.hpp"
#include <gtk/gtk.h>
#include <cairo.h>


void draw(GtkDrawingArea* area, cairo_t* cr, int width, int height, gpointer data);

// void activate_test(GtkApplication* app, gpointer);

void activate(GtkApplication *app, gpointer);
