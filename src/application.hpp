#pragma once

#include "rasterizer/dwrast_rasterizer.hpp"
#include "rasterizer/math/mat4.h"
#include "obj_parser/obj_parser.h"
#include "log.h"
#include <gtk/gtk.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

/**
 * @brief orbit camera, looks at the origin from distance
 * @note angles are in radians
 */
struct camera_t {
    float fov_y = 1.0471976f; // 60 deg
    float near = 0.1f;
    float far = 100.0f;
    float distance = 3.0f;
    float yaw = 0.6f;
    float pitch = 0.35f;
};

/**
 * @brief per vertex buffers for draw_obj_model, kept around so real time rendering doesnt allocate every frame
 */
struct render_scratch_t {
    std::vector<vec3f_t> view_pos;
    std::vector<vertex_t> screen;
    std::vector<uint8_t> visible;
};

struct application_t {
    dwrast::FB2 *frame_buffer_ = nullptr;
    dwrast::DB *depth_buffer = nullptr;

    GtkApplication *gtk_app_ = nullptr;
    GtkWidget *window_ = nullptr;
    GtkWidget *area_ = nullptr;

    bool use_gtk_ = false;
    bool write_ppm = false;
    char ppm_file_name[256];
    bool use_depth_buffer = false;
    uint64_t width = WIDTH;
    uint64_t heigth = HEIGTH;

    const char *obj_file_path = nullptr;
    obj_model model{};
    bool has_model = false;
    mat4f_t model_matrix = mat4f_identity();
    camera_t camera{};
    size_t triangle_count = 0;
    render_scratch_t scratch{};

    uint32_t bench_frames = 0;

    // real time stats for the gtk window title
    int64_t last_frame_time_us = 0;
    int64_t stats_window_start_us = 0;
    uint32_t stats_frames = 0;
    double stats_raster_ms = 0.0;
    uint64_t realtime_frames = 0;
    double realtime_raster_ms = 0.0;
    int64_t realtime_start_us = 0;

    int argc_{0};
    char **argv_;

    int8_t exit_code{0};
};

inline void free_state(application_t *state) {
    if (state->frame_buffer_) {
        dwrast::destroy_FB2(state->frame_buffer_);
        state->frame_buffer_ = nullptr;
    }

    if (state->depth_buffer) {
        dwrast::destroy_DB(state->depth_buffer);
        state->depth_buffer = nullptr;
    }

    if (state->has_model) {
        obj_free(&state->model);
        state->has_model = false;
    }
}

/**
 * @brief builds a matrix that moves the model center to the origin and scales it into a sphere with radius 1
 */
inline mat4f_t fit_model_matrix(const obj_model *model) {
    if (model->vertex_count == 0) return mat4f_identity();

    vec3f_t min = {INFINITY, INFINITY, INFINITY};
    vec3f_t max = {-INFINITY, -INFINITY, -INFINITY};

    for (size_t i = 0; i < model->vertex_count; ++i) {
        const vec4f_t p = model->vertexes[i].pos;
        min = {std::min(min.x, p.x), std::min(min.y, p.y), std::min(min.z, p.z)};
        max = {std::max(max.x, p.x), std::max(max.y, p.y), std::max(max.z, p.z)};
    }

    const vec3f_t center = {(min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f};

    float radius_squared = 0.0f;
    for (size_t i = 0; i < model->vertex_count; ++i) {
        const vec4f_t p = model->vertexes[i].pos;
        radius_squared = std::max(radius_squared, vec3f_length_squared(vec3f_sub({p.x, p.y, p.z}, center)));
    }

    const float radius = std::sqrt(radius_squared);
    const float scale = radius > 0.0f ? 1.0f / radius : 1.0f;

    return mat4f_mul(mat4f_scale(scale), mat4f_translate(-center.x, -center.y, -center.z));
}

/**
 * @brief camera distance at which a sphere with radius 1 fits in the view, with a bit of margin
 * @param aspect width / heigth
 */
inline float fit_camera_distance(const camera_t *camera, const float aspect) {
    const float half_fov_y = camera->fov_y * 0.5f;
    const float half_fov_x = std::atan(std::tan(half_fov_y) * aspect);
    const float half_fov = std::min(half_fov_x, half_fov_y);

    constexpr float margin = 1.15f;
    return margin / std::sin(half_fov);
}

/**
 * @brief transforms, flat shades and rasterizes every face of model
 * @param db optional, without it faces are drawn in file order and overlap wrong
 * @note polygons get fan triangulated, triangles with a vertex behind the near plane are skipped (no clipping yet)
 */
inline void draw_obj_model(dwrast::FB2 *fb, dwrast::DB *db, const obj_model *model, const mat4f_t *model_matrix, const camera_t *camera, render_scratch_t *scratch) {
    const mat4f_t view = mat4f_mul(
        mat4f_translate(0.0f, 0.0f, -camera->distance),
        mat4f_mul(mat4f_rotate_x(camera->pitch), mat4f_rotate_y(camera->yaw))
    );
    const mat4f_t model_view = mat4f_mul(view, *model_matrix);
    const float aspect = static_cast<float>(fb->width) / static_cast<float>(fb->heigth);
    const mat4f_t projection = mat4f_perspective(camera->fov_y, aspect, camera->near, camera->far);

    // transform every vertex once, faces share them
    scratch->view_pos.resize(model->vertex_count);
    scratch->screen.resize(model->vertex_count);
    scratch->visible.resize(model->vertex_count);
    vec3f_t *view_pos = scratch->view_pos.data();
    vertex_t *screen = scratch->screen.data();
    uint8_t *visible = scratch->visible.data();

    for (size_t i = 0; i < model->vertex_count; ++i) {
        const vec4f_t vp = mat4f_mul_vec4f(&model_view, model->vertexes[i].pos);
        view_pos[i] = {vp.x, vp.y, vp.z};

        vertex_t clip = {mat4f_mul_vec4f(&projection, vp), model->vertexes[i].color};
        visible[i] = clip.pos.w > camera->near;
        if (!visible[i]) continue;

        const vec2f_t xy = dwrast::project_vertex(&clip, static_cast<int>(fb->width), static_cast<int>(fb->heigth));
        screen[i] = {{xy.x, xy.y, clip.pos.z / clip.pos.w, clip.pos.w}, clip.color};

        // keeps the float -> int32 casts in calculate_boundingbox() defined
        constexpr float screen_limit = 1.0e7f;
        if (std::fabs(xy.x) > screen_limit || std::fabs(xy.y) > screen_limit) visible[i] = false;
    }

    // light comes from the upper right, a bit from the camera side (view space)
    const vec3f_t light = vec3f_normalize({0.4f, 0.6f, 1.0f});
    constexpr float ambient = 0.15f;

    for (size_t f = 0; f < model->face_count; ++f) {
        const obj_face *face = &model->faces[f];

        for (size_t k = 1; k + 1 < face->count; ++k) {
            const uint32_t i0 = face->indexes[0].v;
            const uint32_t i1 = face->indexes[k].v;
            const uint32_t i2 = face->indexes[k + 1].v;

            if (!visible[i0] || !visible[i1] || !visible[i2]) continue;

            const vec3f_t normal = vec3f_cross(
                vec3f_sub(view_pos[i1], view_pos[i0]),
                vec3f_sub(view_pos[i2], view_pos[i0])
            );
            const float normal_length = vec3f_length(normal);
            if (normal_length == 0.0f) continue;

            // two sided, so models with mixed winding still look right
            const float diffuse = std::fabs(vec3f_dot(normal, light)) / normal_length;
            const color_t color = color_scale(screen[i0].color, ambient + (1.0f - ambient) * diffuse);

            triangle_t tri = {screen[i0], screen[i1], screen[i2]};

            if (db) dwrast::fill_triangle_depth_avx2(fb, db, &tri, color);
            else dwrast::fill_triangle_avx2(fb, &tri, color);
        }
    }
}

inline void draw_triangles(dwrast::FB2 *fb) {
    vertex_t v0 = {
        {600.0f, 180.0f, 0.0f, 1.0f},
        WHITE
    };

    vertex_t v0_1 = {
        {500.0f, 180.0f, 0.0f, 1.0f},
        WHITE
    };

    vertex_t v1 = {
        {500.0f, 260.0f, 0.0f, 1.0f},
        WHITE
    };

    vertex_t v2 = {
        {600.0f, 260.0f, 0.0f, 1.0f},
        WHITE
    };

    vertex_t master_point = {
        {550.0f, 100.0f, 0.0f, 1.0f},
        WHITE
    };

    triangle_t tri1 = { master_point, v1, v0_1 };
    triangle_t tri2 = { master_point, v1, v2 };
    triangle_t tri3 = { master_point, v0_1, v0 };
    triangle_t tri4 = { master_point, v2, v0 };

    dwrast::fill_triangle_avx2(fb, &tri3, RED);
    dwrast::fill_triangle_avx2(fb, &tri1, WHITE);
    dwrast::fill_triangle_avx2(fb, &tri2, GREEN);
    dwrast::fill_triangle_avx2(fb, &tri4, BLUE);
}

/**
 * @brief clears the buffers and draws one frame, the loaded model or the demo triangles
 */
inline void rasterize_frame(application_t *state) {
    dwrast::clear_buf_FB2(state->frame_buffer_, BLACK);
    if (state->depth_buffer) dwrast::clear_DB(state->depth_buffer, INFINITY);

    if (state->has_model) {
        draw_obj_model(state->frame_buffer_, state->depth_buffer, &state->model, &state->model_matrix, &state->camera, &state->scratch);
    } else {
        draw_triangles(state->frame_buffer_);
    }
}

/**
 * @return how long rasterize_frame() took in milliseconds
 */
inline double rasterize_frame_timed(application_t *state) {
    const auto start = std::chrono::steady_clock::now();
    rasterize_frame(state);
    const auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

inline size_t count_model_triangles(const obj_model *model) {
    size_t count = 0;
    for (size_t i = 0; i < model->face_count; ++i) count += model->faces[i].count - 2;
    return count;
}

/**
 * @brief renders bench_frames frames while the camera does one full turn and prints frame time stats
 * @note the camera yaw is restored afterwards
 */
inline void run_benchmark(application_t *state) {
    constexpr uint32_t warmup_frames = 5;
    constexpr float two_pi = 6.28318530718f;
    const float start_yaw = state->camera.yaw;
    const uint32_t frames = state->bench_frames;

    log_bench(ANSI_BOLD "%u" ANSI_RESET " frames" ANSI_GRAY " (+%u warmup) · " ANSI_RESET
              ANSI_BOLD "%lu × %lu" ANSI_RESET ANSI_GRAY " · depth " ANSI_RESET "%s%s" ANSI_RESET ANSI_GRAY " · " ANSI_RESET
              ANSI_BOLD "%zu" ANSI_RESET ANSI_GRAY " triangles per frame" ANSI_RESET,
              frames, warmup_frames, state->width, state->heigth,
              state->depth_buffer ? ANSI_GREEN : ANSI_YELLOW, state->depth_buffer ? "on" : "off",
              state->has_model ? state->triangle_count : static_cast<size_t>(4));

    for (uint32_t i = 0; i < warmup_frames; ++i) rasterize_frame(state);

    std::vector<double> times(frames);
    double total_ms = 0.0;

    for (uint32_t i = 0; i < frames; ++i) {
        state->camera.yaw = start_yaw + two_pi * static_cast<float>(i) / static_cast<float>(frames);
        times[i] = rasterize_frame_timed(state);
        total_ms += times[i];
    }

    state->camera.yaw = start_yaw;
    std::sort(times.begin(), times.end());

    const double avg_ms = total_ms / frames;
    const double median_ms = times[frames / 2];
    const double p99_ms = times[std::min<size_t>(frames - 1, static_cast<size_t>(frames * 0.99))];
    const double fps = 1000.0 / avg_ms;
    const double tris_per_sec = static_cast<double>(state->has_model ? state->triangle_count : 4) * fps;

    log_bench("avg " ANSI_BOLD "%.3f ms" ANSI_RESET ANSI_GRAY "  →  " ANSI_RESET ANSI_BOLD ANSI_GREEN "%.1f fps" ANSI_RESET
              ANSI_GRAY "  ·  " ANSI_RESET ANSI_BOLD "%.2f M" ANSI_RESET " triangles/s",
              avg_ms, fps, tris_per_sec / 1.0e6);
    log_bench(ANSI_GRAY "median " ANSI_RESET "%.3f" ANSI_GRAY "  min " ANSI_RESET "%.3f"
              ANSI_GRAY "  max " ANSI_RESET "%.3f" ANSI_GRAY "  p99 " ANSI_RESET "%.3f" ANSI_GRAY " ms" ANSI_RESET,
              median_ms, times.front(), times.back(), p99_ms);
}

inline void render_frame_gtk(application_t *state) {
    rasterize_frame(state);
    gtk_widget_queue_draw(state->window_);
}
