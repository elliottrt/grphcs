#include "include/geometry.h"

#include <stdio.h>

void calc_tri_normal(geo_t *geo, tri_t *tri) {

    vec3 u = itrn_v3f(vertex(geo, tri->v2)._itrn - vertex(geo, tri->v1)._itrn);
    vec3 v = itrn_v3f(vertex(geo, tri->v3)._itrn - vertex(geo, tri->v1)._itrn);

   tri->normal = v4crossv4(u, v);

#ifdef NORMALIZE_NORMAL
    tri->normal = v4normalized(tri->normal);
#endif

}

static inline void geom_add_tri(geo_t *g, tri_t t) { vec_push(&g->tris, t); }
static inline void geom_add_vert(geo_t *g, vec3 v) { vec_push(&g->verts, v); }
static inline void geom_add_vert3(geo_t *g, float x,  float y, float z) { vec_push(&g->verts, v3f(x, y, z)); }

void new_trii(geo_t *geo, tri_id_t a, tri_id_t b, tri_id_t c) {
    tri_t tri = {{0}, a, b, c};
    calc_tri_normal(geo, &tri);
    geom_add_tri(geo, tri);
}

void geom_print(geo_t *geo) {
    if (geo == NULL) {
        printf("geometry = NULL\n");
        return;
    }
    printf("geometry = {\n");
    tri_t tri; int i;
    vec_foreach(&geo->tris, tri, i) {
        printf("\tTri_%i = (%i, %i, %i),\n", i, tri.v1, tri.v2, tri.v3);
    }
    printf("\n");
    vec3 vert;
    vec_foreach(&geo->verts, vert, i) {
        printf("\tVert_%i = (%f, %f, %f),\n", i, vert.x, vert.y, vert.z);
    }
    printf("}\n");
}

void geom_destroy(geo_t *geo) {
    vec_deinit(&geo->tris);
    vec_deinit(&geo->verts);
    free(geo);
}

geo_t *geom_init() {
    geo_t *geo = malloc(sizeof(geo_t));
    vec_init(&geo->tris);
    vec_init(&geo->verts);
    return geo;
}

geo_t *geom_rect(vec3 dims) {
    geo_t *geo = geom_init();

    float hx = dims.x / 2;
    float hy = dims.y / 2;
    float hz = dims.z / 2;

    geom_add_vert3(geo, -hx, -hy, -hz),
    geom_add_vert3(geo,  hx, -hy, -hz),
    geom_add_vert3(geo,  hx,  hy, -hz),
    geom_add_vert3(geo, -hx,  hy, -hz),
    geom_add_vert3(geo, -hx, -hy,  hz),
    geom_add_vert3(geo,  hx, -hy,  hz),
    geom_add_vert3(geo,  hx,  hy,  hz),
    geom_add_vert3(geo, -hx,  hy,  hz),

    new_trii(geo, 3, 1, 0);
    new_trii(geo, 3, 2, 1);

    new_trii(geo, 4, 5, 7);
    new_trii(geo, 5, 6, 7);
    
    new_trii(geo, 7, 0, 4);
    new_trii(geo, 7, 3, 0);

    new_trii(geo, 2, 6, 1);
    new_trii(geo, 6, 5, 1);

    new_trii(geo, 0, 1, 4);
    new_trii(geo, 1, 5, 4);

    new_trii(geo, 7, 2, 3);
    new_trii(geo, 6, 2, 7);

    return geo;
}

void geom_process_vert(geo_t *geo, char *line) {
    char *section = strtok(line, _OBJ_DELIM);
    float parts[3] = {0};
    int i = 0;

    while(section != NULL)
	{
		if (section[0] != 'v') {
            parts[i++] = atof(section);
        }
		section = strtok(NULL, _OBJ_DELIM);
	}

    geom_add_vert(geo, v3f(parts[0], parts[1], parts[2]));
}

void geom_process_face(geo_t *geo, char *line) {
    char *section = strtok(line, _OBJ_DELIM);
    int i = 0;
    int ids[3] = {0};

    while(section != NULL)
	{
		if (section[0] != 'f') {
            ids[i++] = atoi(section) - 1;
        }
		section = strtok(NULL, _OBJ_DELIM);
	}

    new_trii(geo, ids[0], ids[1], ids[2]);
}

void geom_process_line(geo_t *geo, char *line, ssize_t size) {
    if (size == 0) return;
    switch (*line) {
        case 'v': {
            // just a normal vertex
            if (*(line+1) == ' ') geom_process_vert(geo, line);
        } break;
        // face (tri) data
        case 'f': { 
            geom_process_face(geo, line); 
        } break;
        default: return;
    }
}

geo_t *geom_from_file(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (fp == NULL) {
        fprintf(stderr, "Could not open file '%s'.\n", filepath);
        exit(1);
    }

    geo_t *geo = geom_init();


    while ((read = getline(&line, &len, fp)) != -1) {
        geom_process_line(geo, line, read);
    }

    fclose(fp);
    if (line) free(line);

    return geo;
}

void geom_to_file(geo_t *geo, char *filepath) {
    FILE *out = NULL;
    out = fopen(filepath, "w");
    
    // File Header
    fprintf(out, "# grphcs geometry\n");
    fprintf(out, "# %i vertices\n", geo->verts.length);
    fprintf(out, "# %i triangles\n", geo->tris.length);
    fprintf(out, "\n");

    // Vertices

    int i; vec3 vert;
    vec_foreach(&geo->verts, vert, i) {
        fprintf(out, "v %f %f %f\n", vert.x, vert.y, vert.z);
    }
    fprintf(out, "\n");
    
    // Faces

    tri_t tri;
    vec_foreach(&geo->tris, tri, i) {
        fprintf(out, "f %i %i %i\n", tri.v1 + 1, tri.v2 + 1, tri.v3 + 1);
    }

    fclose(out);
}
