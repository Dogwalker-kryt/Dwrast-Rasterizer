#include <stdio.h>
#include <stdlib.h>
#include <array>
#include "rasterizer/cpu_rasterizer.hpp"
#include "gtkui/ui.hpp"
#include "application.hpp"

using namespace cpu_rast;

constexpr size_t frame_buffer_size = sizeof(FB);

constexpr std::array<const char[16], 16> valid_flags = {
    "-gtk" , "-", "-", "-", "-"
};

int main(int argc, char** argv) {
    printf("Initializing Rasterizer...\n");
    application_t state{};
    
    // cli flags
    {
        if (argc > 1) {
            state.argc_ = argc;
            memcpy(state.argv_, argv, sizeof(char*) * (argc + 1));

            for (uint8_t i = 0; i < argc; ++i) {
                if (strncmp(argv[i], valid_flags[0], 16)) state.use_gtk_ = true;
            }
        }
        printf("[info] set argc:%d arguments to state\n", argc);
    }

    state.frame_buffer_ = new Framebuffer<WIDTH, HEIGTH>;
    printf("[info] allocated bytes:%lu Framebuffer with width:%d heigth:%d\n", frame_buffer_size, WIDTH, HEIGTH);

    printf("Initializing GTK Window...\n");

    state.gtk_app_ = gtk_application_new("com.example.cpurasterizer", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(state.gtk_app_, "activate", G_CALLBACK(activate), &state);
    int status = g_application_run(G_APPLICATION(state.gtk_app_), argc, argv);


    g_object_unref(state.gtk_app_);
    free_state(&state);
    return state.exit_code;
}

/**
 * rm -rf build
 * 
 * cmake -S . -B build -G Ninja
 * 
 * cmake --build build
 * cmake --build build --verbose
 * 
 */