#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include <stdbool.h>

#include "math.h"
#include "../lib/vec.h"

#define NORMALIZE_NORMAL 1
#define _OBJ_DELIM " "

#define vertex(g, v) ((g)->verts.data[(v)])

typedef int tri_id_t;

typedef struct _tri {
    vec4 normal;
    tri_id_t v1, v2, v3;
} tri_t;

typedef vec_t(tri_t) triangles_t;
typedef vec_t(vec4)  vertices_t;

typedef struct _geom {
    triangles_t tris;
    vertices_t verts;
} geo_t;

// Misc Geometry Functions

void geom_print(geo_t *geo);
void geom_destroy(geo_t *geo);

// Premade Geometries (cubes, rectangles, etc)

geo_t *geom_rect(vec4 dims);

// Custom Geometries

geo_t *geom_from_file(const char *filepath);
void geom_to_file(geo_t *geo, char *filepath);

#endif
