#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <array>
#include "rasterizer/dwrast_rasterizer.hpp"
#include "gtkui/ui.hpp"
#include "application.hpp"

using namespace dwrast;

constexpr const char* valid_flags[16] = {
    "-gtk" , "-ppm", "-width", "-height", "-w", "-h", "-obj", "-depth", "-z"
};

#define GTK_FLAG valid_flags[0]
#define PPM_FLAG valid_flags[1]
#define WIDTH_FLAG valid_flags[2]
#define HEIGHT_FLAG valid_flags[3]
#define SMALL_HEIGHT_FLAG valid_flags[5]
#define SMALL_WIDTH_FLAG valid_flags[4]
#define OBJ_FILE_FLAG valid_flags[6]
#define DEPTH_BUFFER_FLAG valid_flags[7]
#define SMALL_DEPTH_BUFFER_FLAG valid_flags[8]

int main(int argc, char** argv) {
    printf("%sInitializing Rasterizer...%s\n", BOLD_ANSI, RESET_ANSI);
    application_t state{};
    size_t frame_buffer_size_dyn;

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
                memcpy(&state.ppm_file_name, argv[i + 1], sizeof(state.ppm_file_name));
                state.ppm_file_name[sizeof(state.ppm_file_name) - 1] = '\0';
                i++;
            }

            if (strncmp(argv[i], WIDTH_FLAG, 16) == 0 || strncmp(argv[i], SMALL_WIDTH_FLAG, 16) == 0) {
                if (i + 1 >= argc) {
                    printf("%s[ERROR]%s no width argument entered\n", RED_ANSI, RESET_ANSI);
                    state.exit_code = -1;
                    goto EXIT;
                }

                char *endptr = nullptr;
                state.width = strtoull(argv[i + 1], &endptr, 10);
                i++;
            }

            if (strncmp(argv[i], HEIGHT_FLAG, 16) == 0 || strncmp(argv[i], SMALL_HEIGHT_FLAG, 16) == 0) {
                if (i + 1 >= argc) {
                    printf("%s[ERROR]%s no height argument entered\n", RED_ANSI, RESET_ANSI);
                    state.exit_code = -1;
                    goto EXIT;
                }

                char *endptr = nullptr;
                state.heigth = strtoull(argv[i + 1], &endptr, 10);
                i++;
            }

            if (strncmp(argv[i], OBJ_FILE_FLAG, 16) == 0) {
                if (i + 1 >= argc) {
                    printf("%s[ERROR]%s no obj file path entered\n", RED_ANSI, RESET_ANSI);
                    state.exit_code = -1;
                    goto EXIT;
                }
            }

            if (strncmp(argv[i], DEPTH_BUFFER_FLAG, 16) == 0 || strncmp(argv[i], SMALL_DEPTH_BUFFER_FLAG, 16) == 0) {
                state.use_depth_buffer = true;
            }
        }
       
    } 

SKIP_FLAGS:
    state.frame_buffer_ = dwrast::create_FB2(state.width, state.heigth);
    frame_buffer_size_dyn = state.width * state.heigth;

    dwrast::clear_buf_FB2(state.frame_buffer_, BLACK);
    printf("%s[info]%s allocated %sbytes:%lu%s Framebuffer with %swidth:%lu heigth:%lu%s\n", BOLD_ANSI, RESET_ANSI, BOLD_ANSI, frame_buffer_size_dyn, RESET_ANSI, BOLD_ANSI, state.width, state.heigth, RESET_ANSI);

    if (state.use_depth_buffer) {
        state.depth_buffer = dwrast::create_DB(state.width, state.heigth);
        printf("%s[info]%s allocated DB with same params as FB2\n", BOLD_ANSI, RESET_ANSI);
    }

    if (state.use_gtk_) {
        printf("%sInitializing GTK Window...%s\n", BOLD_ANSI, RESET_ANSI);

        state.gtk_app_ = gtk_application_new("com.example.cpurasterizer", G_APPLICATION_DEFAULT_FLAGS);
        printf("%s[info]%s created new gtk application\n", BOLD_ANSI, RESET_ANSI);

        g_signal_connect(state.gtk_app_, "activate", G_CALLBACK(activate), &state);
        (void)g_application_run(G_APPLICATION(state.gtk_app_), 0, NULL);
        printf("%s[info]%s connected callback and started gtk application\n", BOLD_ANSI, RESET_ANSI);

        g_object_unref(state.gtk_app_);
        printf("%s[info]%s unrefed state.gtk_app_\n", BOLD_ANSI, RESET_ANSI);
    } else {
        // rasterisation in no gtk mode
        draw_triangles(state.frame_buffer_);
    }

    if (state.write_ppm) {
        const bool ok = dwrast::write_ppm_FB2(state.frame_buffer_, state.ppm_file_name);
        printf("%s[info]%s writing PPM output to %s%s%s -> %s%s %s\n",
               BOLD_ANSI, RESET_ANSI,
               BOLD_ANSI, state.ppm_file_name, RESET_ANSI,
               ok ? GREEN_ANSI : RED_ANSI,
               ok ? "success" : "failed", RESET_ANSI);
    }

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