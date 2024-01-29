#ifndef _GRPHCS_DEFS_H
#define _GRPHCS_DEFS_H

#include "input.hpp"
#include "math.hpp"

#include <cstdint>
#include <vector>

// Whether we should use the lock texture method when setting the pixels;
#define USE_LOCK_TEXTURE true
// Whether the game should have a buffer for picking faces and geometries
#define USE_PICK_BUFFER false
// Should we cull back facing triangles in geometries?
#define CULL_BACK_FACING true

#define FARPLANE 1000.f
#define NEARPLANE 0.1f
#define VIEWANGLE 90.f
#define AMBIENT_LIGHT 0.05f

/* Graphics Definitions */

struct GrphcsPixel {
    uint8_t b, g, r, a;

    GrphcsPixel(uint8_t r, uint8_t g, uint8_t b) {
        this->r = r; this->g = g; this->b = b;
        this->a = SDL_ALPHA_OPAQUE;
    }

    GrphcsPixel(uint8_t * rgb) {
        this->r = rgb[0];
        this->g = rgb[1];
        this->b = rgb[2];
    }

    GrphcsPixel operator*(float f) {
        return GrphcsPixel(this->r * f, this->g * f, this->b * f);
    }

    void operator+=(GrphcsPixel &other) {
        this->r += other.r;
        this->g += other.g;
        this->b += other.b;
    }

};

#define DEFAULT_PIXEL GrphcsPixel(0, 0, 0)
#define NO_MATERIAL_COLOR GrphcsPixel(233, 0, 236)

typedef int64_t grphcs_pickbuffer_t;
typedef int32_t grphcs_pickbuffer_id_t;


/* Geometry Definitions */

typedef int32_t grphcs_tri_id_t;
typedef int32_t grphcs_material_id_t;
typedef int32_t grphcs_geometry_id_t;


#endif
