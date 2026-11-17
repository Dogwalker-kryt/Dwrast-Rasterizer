#pragma once

#include "../rasterizer/vector/triangle.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief marks a missing vt or vn index in a face (e.g. "f 1//3" has no vt)
 */
#define OBJ_IDX_NONE UINT32_MAX

/**
 * @brief one corner of a face, all indexes are 0 based (already converted from the 1 based obj format)
 */
typedef struct obj_list_idx {
    uint32_t v;
    uint32_t vt;
    uint32_t vn;

} obj_list_idx;

/**
 * @brief polygon with count corners (count >= 3), not triangulated
 */
typedef struct obj_face {
    obj_list_idx *indexes;
    size_t count;

} obj_face;

typedef struct obj_model {
    vertex_t *vertexes;
    size_t vertex_count;

    vec2f_t *texcoords;
    size_t texcoords_count;

    vec3f_t *normals;
    size_t normals_count;

    obj_face *faces;
    size_t face_count;

} obj_model;

/**
 * @brief parses v, vt, vn and f lines from an already opened obj file, everything else (o, g, s, usemtl, ...) is skipped
 * @param file opened obj file
 * @param model gets zeroed and filled, must be freed with obj_free() even on failure
 * @return false on malformed input or out of memory
 * @warning uses strtof, so the LC_NUMERIC locale must be "C" (gtk changes it, so parse before starting gtk)
 */
bool obj_parse_file(FILE *file, obj_model *model);

/**
 * @brief opens path and calls obj_parse_file()
 */
bool obj_load(const char *path, obj_model *model);

void obj_free(obj_model *model);

#ifdef __cplusplus
}
#endif
