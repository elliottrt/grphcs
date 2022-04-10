#ifndef _DRAW3D_H
#define _DRAW3D_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "geometry.h"

typedef enum DrawMode {
    DM_ORTHOGRAPHIC = 0,
    DM_PERSPECTIVE = 1,
} DrawMode;

void draw3d_init();

#define PICKBUFFER_TYPE int32_t

void zero_buffers();
void draw_depthbuf();
void draw_pickbuf();

PICKBUFFER_TYPE get_vertex(void);

int draw_setup(DrawMode drawmode, float farplane);

// Primitive 3D Drawing

void point3d(vec3 p);
void line3d(vec3 a, vec3 b);
void triangle3d(vec3 a, vec3 b, vec3 c, bool fill);

// Geometry 3D Drawing ---------------------------------------

int draw_geometry(geo_t *geo, vec3 pos, bool fill);

void rotate(float x, float y, float z);
void rotate_relative(float x, float y, float z);
void scale(float x, float y, float z);

#endif
