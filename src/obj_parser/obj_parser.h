#pragma once

#include "../rasterizer/vector/triangle.h" 
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct obj_list_idx {
    uint32_t v;
    uint32_t vt;
    uint32_t vn;

} obj_list_idx;

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

} obj_model;



void get_objects_file_content(FILE *file);