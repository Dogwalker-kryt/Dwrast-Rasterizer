#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <array>
#include "rasterizer/dwrast_rasterizer.hpp"
#include "gtkui/ui.hpp"
#include "application.hpp"

using namespace dwrast;

constexpr size_t frame_buffer_size = sizeof(FB);
static size_t frame_buffer_size_dyn;

constexpr std::array<const char[16], 16> valid_flags = {
    "-gtk" , "-ppm", "-width", "-heigth", "-"
};

#define GTK_FLAG valid_flags[0]
#define PPM_FLAG valid_flags[1]
#define WIDTH_FLAG valid_flags[2]
#define HEIGHT_FLAG valid_flags[3]

__always_inline void set_state_custom_FB_true(application_t *state) {
    state->use_custom_FB_args = true;
}


int main(int argc, char** argv) {
    printf("%sInitializing Rasterizer...%s\n", BOLD_ANSI, RESET_ANSI);
    application_t state{};
    frame_buffer_size_dyn = frame_buffer_size;

    // cli flags
    {
        if (argc < 2) { goto SKIP_FLAGS; }

        state.argc_ = argc;
        printf("%s[info]%s set %sargc:%d%s arguments to state\n", BOLD_ANSI, RESET_ANSI, BOLD_ANSI, argc, RESET_ANSI);
        state.argv_ = argv;
        printf("%s[info]%s set state.argv to argv\n", BOLD_ANSI, RESET_ANSI);

        for (uint8_t i = 0; i < argc; ++i) {
            if (strncmp(argv[i], GTK_FLAG, 16) == 0) state.use_gtk_ = true;

            if (strncmp(argv[i], PPM_FLAG, 16) == 0) {
                if (i + 1 >= argc) {
                    printf("%s[ERROR]%s no filename entered\n", RED_ANSI, RESET_ANSI);
                    state.exit_code = -1;
                    goto EXIT;
                }

                state.write_ppm = true;
                memcpy(&state.file_name, argv[i + 1], sizeof(state.file_name));
                state.file_name[sizeof(state.file_name) - 1] = '\0';
                i++;
            }

            if (strncmp(argv[i], WIDTH_FLAG, 16) == 0) {
                if (i + 1 >= argc) {
                    printf("%s[ERROR]%s no width argument entered\n", RED_ANSI, RESET_ANSI);
                    state.exit_code = -1;
                    goto EXIT;
                }

                set_state_custom_FB_true(&state);
                char *endptr = nullptr;
                state.width = strtoull(argv[i + 1], &endptr, 10);
                i++;
            }

            if (strncmp(argv[i], HEIGHT_FLAG, 16) == 0) {
                if (i + 1 >= argc) {
                    printf("%s[ERROR]%s no height argument entered\n", RED_ANSI, RESET_ANSI);
                    state.exit_code = -1;
                    goto EXIT;
                }

                set_state_custom_FB_true(&state);
                char *endptr = nullptr;
                state.heigth = strtoull(argv[i + 1], &endptr, 10);
                i++;
            }
        }
        
    }

SKIP_FLAGS:
    if (state.use_custom_FB_args) {
        state.frame_buffer_ = dwrast::create_FB2(state.width, state.heigth);
        frame_buffer_size_dyn = state.width * state.heigth;
    } else {
        state.frame_buffer_ = dwrast::create_FB2(WIDTH, HEIGTH);
    }

    printf("%s[info]%s allocated %sbytes:%lu%s Framebuffer with %swidth:%lu heigth:%lu%s\n", BOLD_ANSI, RESET_ANSI, BOLD_ANSI, frame_buffer_size_dyn, RESET_ANSI, BOLD_ANSI, state.width, state.heigth, RESET_ANSI);

    if (state.use_gtk_) {
        printf("%sInitializing GTK Window...%s\n", BOLD_ANSI, RESET_ANSI);

        state.gtk_app_ = gtk_application_new("com.example.cpurasterizer", G_APPLICATION_DEFAULT_FLAGS);
        printf("%s[info]%s created new gtk application\n", BOLD_ANSI, RESET_ANSI);

        g_signal_connect(state.gtk_app_, "activate", G_CALLBACK(activate), &state);
        int status = g_application_run(G_APPLICATION(state.gtk_app_), 0, NULL);
        printf("%s[info]%s connected callback and started gtk application\n", BOLD_ANSI, RESET_ANSI);

        g_object_unref(state.gtk_app_);
        printf("%s[info]%s unrefed state.gtk_app_\n", BOLD_ANSI, RESET_ANSI);
    }

    // state.frame_buffer_->clear(BLACK);

    // // draw_triangles(state.frame_buffer_);

    // state.frame_buffer_->write_ppm("test.ppm");

    EXIT:
        free_state(&state);
        printf("%s[EXIT]%s exited with %scode:%d%s\n",BOLD_ANSI, RESET_ANSI, BOLD_ANSI, state.exit_code, RESET_ANSI);
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