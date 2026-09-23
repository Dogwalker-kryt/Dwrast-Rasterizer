#include <stdio.h>
#include <stdlib.h>
#include <array>
#include "rasterizer/cpu_rasterizer.hpp"
#include "gtkui/ui.hpp"
#include "application.hpp"
#include <iostream>

using namespace dwrast;

constexpr size_t frame_buffer_size = sizeof(FB);

constexpr std::array<const char[16], 16> valid_flags = {
    "-gtk" , "-ppm", "-", "-", "-"
};

int main(int argc, char** argv) {
    printf("Initializing Rasterizer...\n");
    application_t state{};

    // cli flags
    {
        if (argc > 1) {
            state.argc_ = argc;
            printf("[info] set argc:%d arguments to state\n", argc);
            state.argv_ = argv;
            printf("[info] set state.argv to argv\n");

            for (uint8_t i = 0; i < argc; ++i) {
                if (strncmp(argv[i], valid_flags[0], 16) == 0) state.use_gtk_ = true;

                if (strncmp(argv[i], valid_flags[1], 16) == 0) {
                    if (i + 1 >= argc) {
                        printf("%s[error]%s no filename entered\n", RED_ANSI, RESET_ANSI);
                        state.exit_code = -1;
                        goto EXIT;
                    }

                    state.write_ppm = true;
                    memcpy(&state.file_name, argv[i + 1], sizeof(state.file_name));
                    state.file_name[sizeof(state.file_name) - 1] = '\0';
                    i++;
                }
            }
        }
    }

    state.frame_buffer_ = new Framebuffer<WIDTH, HEIGTH>;
    printf("[info] allocated bytes:%lu Framebuffer with width:%d heigth:%d\n", frame_buffer_size, WIDTH, HEIGTH);

    if (state.use_gtk_) {
        printf("Initializing GTK Window...\n");

        state.gtk_app_ = gtk_application_new("com.example.cpurasterizer", G_APPLICATION_DEFAULT_FLAGS);
        printf("[info] created new gtk application\n");

        g_signal_connect(state.gtk_app_, "activate", G_CALLBACK(activate), &state);
        int status = g_application_run(G_APPLICATION(state.gtk_app_), 0, NULL);
        printf("[info] connected callback and started gtk application\n");

        g_object_unref(state.gtk_app_);
        printf("[info] unrefed state.gtk_app_\n");
    }

    // state.frame_buffer_->clear(BLACK);

    // // draw_triangles(state.frame_buffer_);

    // state.frame_buffer_->write_ppm("test.ppm");

    EXIT:
        free_state(&state);
        printf("[exit] exited with code:%d\n", state.exit_code);
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