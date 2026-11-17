#include "obj_parser.h"
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define OBJ_LINE_MAX 4096
#define OBJ_MAX_FLOATS 7

static bool grow_array(void **array, size_t *capacity, const size_t needed, const size_t elem_size) {
    if (needed <= *capacity) return true;

    size_t new_capacity = *capacity ? *capacity * 2 : 64;
    while (new_capacity < needed) new_capacity *= 2;
    if (new_capacity > SIZE_MAX / elem_size) return false;

    void *grown = realloc(*array, new_capacity * elem_size);
    if (!grown) return false;

    *array = grown;
    *capacity = new_capacity;
    return true;
}

static inline const char *skip_blank(const char *s) {
    while (*s == ' ' || *s == '\t') ++s;
    return s;
}

static inline bool is_line_end(const char c) {
    return c == '\0' || c == '\n' || c == '\r' || c == '#';
}

/**
 * @brief reads up to max floats, stops at the first thing that isnt a number
 * @return amount of floats read, -1 if one of them is nan/inf or there is junk after them
 */
static int parse_floats(const char *s, float *out, const int max) {
    int count = 0;

    while (count < max) {
        s = skip_blank(s);
        if (is_line_end(*s)) break;

        char *end = NULL;
        const float value = strtof(s, &end);
        if (end == s || !isfinite(value)) return -1;

        out[count++] = value;
        s = end;
    }

    s = skip_blank(s);
    return is_line_end(*s) ? count : -1;
}

/**
 * @brief turns a 1 based (or negative = relative to the end) obj index into a 0 based one
 */
static bool resolve_index(const long raw, const size_t count, uint32_t *out) {
    if (count >= OBJ_IDX_NONE) return false;

    if (raw > 0 && (size_t)raw <= count) {
        *out = (uint32_t)(raw - 1);
        return true;
    }

    if (raw < 0 && (size_t)(-raw) <= count) {
        *out = (uint32_t)(count - (size_t)(-raw));
        return true;
    }

    return false;
}

/**
 * @brief parses one "f" line, supports v, v/vt, v//vn and v/vt/vn corners
 */
static bool parse_face(const char *s, const obj_model *model, obj_face *face) {
    size_t capacity = 0;
    face->indexes = NULL;
    face->count = 0;

    while (true) {
        s = skip_blank(s);
        if (is_line_end(*s)) break;

        obj_list_idx idx = {OBJ_IDX_NONE, OBJ_IDX_NONE, OBJ_IDX_NONE};
        char *end = NULL;

        long raw = strtol(s, &end, 10);
        if (end == s || !resolve_index(raw, model->vertex_count, &idx.v)) goto FAIL;
        s = end;

        if (*s == '/') {
            ++s;
            if (*s != '/') {
                raw = strtol(s, &end, 10);
                if (end == s || !resolve_index(raw, model->texcoords_count, &idx.vt)) goto FAIL;
                s = end;
            }

            if (*s == '/') {
                ++s;
                raw = strtol(s, &end, 10);
                if (end == s || !resolve_index(raw, model->normals_count, &idx.vn)) goto FAIL;
                s = end;
            }
        }

        if (!is_line_end(*s) && *s != ' ' && *s != '\t') goto FAIL;

        if (!grow_array((void**)&face->indexes, &capacity, face->count + 1, sizeof(obj_list_idx))) goto FAIL;
        face->indexes[face->count++] = idx;
    }

    if (face->count >= 3) return true;

FAIL:
    free(face->indexes);
    face->indexes = NULL;
    face->count = 0;
    return false;
}

static inline uint32_t float_to_channel(const float value) {
    const float clamped = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
    return (uint32_t)(clamped * 255.0f + 0.5f);
}

bool obj_parse_file(FILE *file, obj_model *model) {
    memset(model, 0, sizeof(*model));
    if (!file) return false;

    size_t vertex_capacity = 0;
    size_t texcoords_capacity = 0;
    size_t normals_capacity = 0;
    size_t face_capacity = 0;

    char line[OBJ_LINE_MAX];
    size_t line_number = 0;
    float values[OBJ_MAX_FLOATS];

    while (fgets(line, sizeof(line), file) != NULL) {
        ++line_number;

        const size_t len = strlen(line);
        if (len == sizeof(line) - 1 && line[len - 1] != '\n' && !feof(file)) {
            printf("%s[ERROR]%s obj line %zu is longer than %d chars\n", RED_ANSI, RESET_ANSI, line_number, OBJ_LINE_MAX - 1);
            return false;
        }

        const char *s = skip_blank(line);

        // v x y z [w]  or  v x y z r g b (vertex colors, non standard but common)
        if (s[0] == 'v' && (s[1] == ' ' || s[1] == '\t')) {
            const int count = parse_floats(s + 2, values, OBJ_MAX_FLOATS);
            if (count != 3 && count != 4 && count != 6) goto PARSE_ERROR;
            if (!grow_array((void**)&model->vertexes, &vertex_capacity, model->vertex_count + 1, sizeof(vertex_t))) goto OOM;

            // w is only a weight for rational curves, for meshes it is always 1
            color_t color = WHITE;
            if (count == 6) {
                color = COLOR(float_to_channel(values[3]), float_to_channel(values[4]), float_to_channel(values[5]));
            }

            model->vertexes[model->vertex_count++] = VEC_INIT(vertex_t,
                VEC_INIT(vec4f_t, values[0], values[1], values[2], 1.0f),
                color
            );
            continue;
        }

        // vt u [v [w]]
        if (s[0] == 'v' && s[1] == 't' && (s[2] == ' ' || s[2] == '\t')) {
            const int count = parse_floats(s + 3, values, 3);
            if (count < 1) goto PARSE_ERROR;
            if (!grow_array((void**)&model->texcoords, &texcoords_capacity, model->texcoords_count + 1, sizeof(vec2f_t))) goto OOM;

            model->texcoords[model->texcoords_count++] = VEC_INIT(vec2f_t, values[0], count > 1 ? values[1] : 0.0f);
            continue;
        }

        // vn x y z
        if (s[0] == 'v' && s[1] == 'n' && (s[2] == ' ' || s[2] == '\t')) {
            if (parse_floats(s + 3, values, 3) != 3) goto PARSE_ERROR;
            if (!grow_array((void**)&model->normals, &normals_capacity, model->normals_count + 1, sizeof(vec3f_t))) goto OOM;

            model->normals[model->normals_count++] = VEC_INIT(vec3f_t, values[0], values[1], values[2]);
            continue;
        }

        // f a b c ...
        if (s[0] == 'f' && (s[1] == ' ' || s[1] == '\t')) {
            if (!grow_array((void**)&model->faces, &face_capacity, model->face_count + 1, sizeof(obj_face))) goto OOM;
            if (!parse_face(s + 2, model, &model->faces[model->face_count])) goto PARSE_ERROR;

            model->face_count++;
            continue;
        }

        // comments, empty lines, o, g, s, usemtl, mtllib, l, p, ... are ignored for now
    }

    if (ferror(file)) {
        printf("%s[ERROR]%s failed reading obj file\n", RED_ANSI, RESET_ANSI);
        return false;
    }

    return true;

PARSE_ERROR:
    printf("%s[ERROR]%s malformed obj at line %zu: %s", RED_ANSI, RESET_ANSI, line_number, line);
    if (line[0] == '\0' || line[strlen(line) - 1] != '\n') putchar('\n');
    return false;

OOM:
    printf("%s[ERROR]%s out of memory while parsing obj at line %zu\n", RED_ANSI, RESET_ANSI, line_number);
    return false;
}

bool obj_load(const char *path, obj_model *model) {
    FILE *file = fopen(path, "r");
    if (!file) {
        memset(model, 0, sizeof(*model));
        printf("%s[ERROR]%s could not open obj file %s%s%s\n", RED_ANSI, RESET_ANSI, BOLD_ANSI, path, RESET_ANSI);
        return false;
    }

    const bool ok = obj_parse_file(file, model);
    fclose(file);
    return ok;
}

void obj_free(obj_model *model) {
    if (!model) return;

    for (size_t i = 0; i < model->face_count; ++i) free(model->faces[i].indexes);

    free(model->faces);
    free(model->vertexes);
    free(model->texcoords);
    free(model->normals);
    memset(model, 0, sizeof(*model));
}
