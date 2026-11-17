#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <array>
#include "rasterizer/dwrast_rasterizer.hpp"
#include "gtkui/ui.hpp"
#include "application.hpp"
#include "log.h"

#ifndef DWRAST_VERSION
#define DWRAST_VERSION "dev"
#endif

using namespace dwrast;

constexpr const char* valid_flags[16] = {
    "-gtk" , "-ppm", "-width", "-height", "-w", "-h", "-obj", "-depth", "-z", "-bench"
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
#define BENCH_FLAG valid_flags[9]

int main(int argc, char** argv) {
    log_header(DWRAST_VERSION);
    application_t state{};
    size_t frame_buffer_size_dyn;

    {
        if (argc < 2) { goto SKIP_FLAGS; }

        state.argc_ = argc;
        state.argv_ = argv;
        log_debug("set state.argc_ = %d and state.argv_", argc);

        char *argv_str = nullptr;
        for (uint8_t i = 0; i < argc; ++i) {
            argv_str = argv[i];

            if (strncmp(argv_str, GTK_FLAG, 16) == 0) state.use_gtk_ = true;

            if (strncmp(argv_str, PPM_FLAG, 16) == 0) {
                if (i + 1 >= argc) {
                    log_error(ANSI_BOLD "-ppm" ANSI_RESET " needs a file name");
                    state.exit_code = -1;
                    goto EXIT;
                }

                state.write_ppm = true;
                if (snprintf(state.ppm_file_name, sizeof(state.ppm_file_name), "%s", argv[i + 1]) >= static_cast<int>(sizeof(state.ppm_file_name))) {
                    log_error("ppm file name is longer than " ANSI_BOLD "%zu" ANSI_RESET " chars", sizeof(state.ppm_file_name) - 1);
                    state.exit_code = -1;
                    goto EXIT;
                }
                i++;
            }

            if (strncmp(argv_str, WIDTH_FLAG, 16) == 0 || strncmp(argv_str, SMALL_WIDTH_FLAG, 16) == 0) {
                if (i + 1 >= argc) {
                    log_error(ANSI_BOLD "-width / -w" ANSI_RESET " needs a value");
                    state.exit_code = -1;
                    goto EXIT;
                }

                char *endptr = nullptr;
                state.width = strtoull(argv[i + 1], &endptr, 10);
                i++;
            }

            if (strncmp(argv_str, HEIGHT_FLAG, 16) == 0 || strncmp(argv_str, SMALL_HEIGHT_FLAG, 16) == 0) {
                if (i + 1 >= argc) {
                    log_error(ANSI_BOLD "-height / -h" ANSI_RESET " needs a value");
                    state.exit_code = -1;
                    goto EXIT;
                }

                char *endptr = nullptr;
                state.heigth = strtoull(argv[i + 1], &endptr, 10);
                i++;
            }

            if (strncmp(argv_str, OBJ_FILE_FLAG, 16) == 0) {
                if (i + 1 >= argc) {
                    log_error(ANSI_BOLD "-obj" ANSI_RESET " needs a file path");
                    state.exit_code = -1;
                    goto EXIT;
                }

                state.obj_file_path = argv[i + 1];
                i++;
            }

            if (strncmp(argv_str, DEPTH_BUFFER_FLAG, 16) == 0 || strncmp(argv_str, SMALL_DEPTH_BUFFER_FLAG, 16) == 0) {
                state.use_depth_buffer = true;
            }

            if (strncmp(argv_str, BENCH_FLAG, 16) == 0) {
                char *endptr = nullptr;
                const unsigned long frames = (i + 1 < argc) ? strtoul(argv[i + 1], &endptr, 10) : 0;

                if (i + 1 >= argc || endptr == argv[i + 1] || *endptr != '\0' || frames == 0 || frames > 1000000) {
                    log_error(ANSI_BOLD "-bench" ANSI_RESET " needs a frame count between " ANSI_BOLD "1" ANSI_RESET " and " ANSI_BOLD "1000000" ANSI_RESET);
                    state.exit_code = -1;
                    goto EXIT;
                }

                state.bench_frames = static_cast<uint32_t>(frames);
                i++;
            }
        }
       
    } 

SKIP_FLAGS:
    state.frame_buffer_ = dwrast::create_FB2(state.width, state.heigth);
    frame_buffer_size_dyn = state.width * state.heigth;

    dwrast::clear_buf_FB2(state.frame_buffer_, BLACK);
    log_info("framebuffer   " ANSI_BOLD "%lu × %lu" ANSI_RESET ANSI_GRAY "   %zu px · %.1f MiB" ANSI_RESET,
             state.width, state.heigth, frame_buffer_size_dyn, frame_buffer_size_dyn * sizeof(uint32_t) / (1024.0 * 1024.0));

    if (state.use_depth_buffer) {
        state.depth_buffer = dwrast::create_DB(state.width, state.heigth);
        log_info("depth buffer  " ANSI_BOLD ANSI_GREEN "on" ANSI_RESET ANSI_GRAY "   %.1f MiB" ANSI_RESET,
                 frame_buffer_size_dyn * sizeof(float) / (1024.0 * 1024.0));
    }

    // must happen before gtk starts, gtk sets the locale and strtof would break on "1.5" with e.g. de_DE
    if (state.obj_file_path) {
        if (!obj_load(state.obj_file_path, &state.model)) {
            obj_free(&state.model);
            state.exit_code = -1;
            goto EXIT;
        }

        state.has_model = true;
        state.triangle_count = count_model_triangles(&state.model);
        state.model_matrix = fit_model_matrix(&state.model);
        state.camera.distance = fit_camera_distance(&state.camera, static_cast<float>(state.width) / static_cast<float>(state.heigth));
        log_info("model         " ANSI_BOLD "%s" ANSI_RESET, state.obj_file_path);
        log_info("              " ANSI_BOLD "%zu" ANSI_RESET ANSI_GRAY " vertexes · " ANSI_RESET
                 ANSI_BOLD "%zu" ANSI_RESET ANSI_GRAY " faces · " ANSI_RESET
                 ANSI_BOLD "%zu" ANSI_RESET ANSI_GRAY " triangles" ANSI_RESET,
                 state.model.vertex_count, state.model.face_count, state.triangle_count);

        if (!state.use_depth_buffer) {
            log_warn("no depth buffer " ANSI_GRAY "(-depth / -z)" ANSI_RESET ", faces are drawn in file order and will overlap wrong");
        }
    }

    if (state.bench_frames) run_benchmark(&state);

    if (state.use_gtk_) {
        log_gtk("opening window " ANSI_GRAY "(close it or press ctrl+c to quit)" ANSI_RESET);

        state.gtk_app_ = gtk_application_new("com.example.cpurasterizer", G_APPLICATION_DEFAULT_FLAGS);
        log_debug("created gtk application");

        g_signal_connect(state.gtk_app_, "activate", G_CALLBACK(activate), &state);
        (void)g_application_run(G_APPLICATION(state.gtk_app_), 0, NULL);
        log_debug("gtk application finished");
        print_realtime_stats(&state);

        g_object_unref(state.gtk_app_);
        log_debug("unrefed state.gtk_app_");
        
    } else {
        // rasterisation in no gtk mode
        rasterize_frame(&state);
    }

    if (state.write_ppm) {
        const bool ok = dwrast::write_ppm_FB2(state.frame_buffer_, state.ppm_file_name);
        if (ok) log_ok("wrote ppm " ANSI_BOLD "%s" ANSI_RESET, state.ppm_file_name);
        else log_error("could not write ppm " ANSI_BOLD "%s" ANSI_RESET, state.ppm_file_name);
    }

    EXIT:
        free_state(&state);
        log_exit("exited with code " ANSI_RESET ANSI_BOLD "%s%d" ANSI_RESET, state.exit_code == 0 ? ANSI_GREEN : ANSI_RED, state.exit_code);
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