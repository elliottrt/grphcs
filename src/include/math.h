#ifndef _MATH_H
#define _MATH_H

#include <math.h>
// SSE
//#include <xmmintrin.h>
// SSE2
//#include <emmintrin.h>
// SSE3
//#include <pmmintrin.h>
// SSSE3
//#include <tmmintrin.h>
// SSE4
#include <immintrin.h>
//#include <smmintrin.h>

#include <stdio.h>

typedef __v4sf _itrn_v4;

typedef union _v4f {
    struct { float x, y, z, w; };
    _itrn_v4 _itrn;
} vec4;

typedef vec4 vec3;

typedef struct _v2f {
    float x, y;
} vec2;

#define v4f(X, Y, Z, W) ((vec4) { .x = (X), .y = (Y), .z = (Z), .w = (W)})
#define v3f(X, Y, Z) ((vec4) { .x = (X), .y = (Y), .z = (Z), .w = 0})
#define itrn_v4f(_i) ((vec4) { ._itrn = (_i) })
#define itrn_v3f(_i) itrn_v4f(_i)

// Vector Math

static inline float v4dotv4(vec4 a, vec4 b) { return (_mm_dp_ps(a._itrn, b._itrn, 0xFF))[0]; }

static inline float v4sqrmag(vec4 a) { return v4dotv4(a, a); }

static inline float v4mag(vec4 a) { return sqrtf(v4sqrmag(a)); }

static inline vec4  v4crossv4(vec4 a, vec4 b) { 
    return (vec4) { 
        .x = a.y * b.z - a.z * b.y, 
        .y = a.z * b.x - a.x * b.z, 
        .z = a.x * b.y - a.y * b.x, 
        .w = 0
    }; 
}

static inline vec4  v4normalized(vec4 v) { return (vec4) { ._itrn = v._itrn / v4mag(v) }; }

static inline vec4  v3avg2(vec4 a, vec4 b) { return (vec4) { ._itrn = (a._itrn + b._itrn) / 2.f }; }

static inline vec4  v3avg3(vec4 a, vec4 b, vec4 c) { return (vec4) { ._itrn = (a._itrn + b._itrn + c._itrn) / 3.f }; }

// Matrix Math

typedef struct _m4 {
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
} mat4;

#define MAT4_IDENTITY ((mat4) { \
    1, 0, 0, 0, \
    0, 1, 0, 0, \
    0, 0, 1, 0, \
    0, 0, 0, 1  \
})

typedef struct _rotation {
    mat4 rotm;
    vec3 angles;
} rotation_t;

static inline vec4 m4mulv4(mat4 a, vec4 b) {
    return (vec4){
        .x = b.x * a.m00 + b.y * a.m10 + b.z * a.m20 + b.w * a.m30,
        .y = b.x * a.m01 + b.y * a.m11 + b.z * a.m21 + b.w * a.m31,
        .z = b.x * a.m02 + b.y * a.m12 + b.z * a.m22 + b.w * a.m32,
        .w = b.x * a.m03 + b.y * a.m13 + b.z * a.m23 + b.w * a.m33,
    };
}

static inline rotation_t rotation_create(vec3 rotation) {

    rotation_t rot = { .rotm = MAT4_IDENTITY, .angles = rotation};

    float xsin = sinf(rotation.x), ysin = sinf(rotation.y), zsin = sinf(rotation.z);
    float xcos = cosf(rotation.x), ycos = cosf(rotation.y), zcos = cosf(rotation.z);
    rot.rotm = (mat4) {
        .m00 = xcos * ycos, 
        .m01 = (xcos * ysin * zsin) - (xsin * zcos), 
        .m02 = (xcos * ysin * zcos) + (xsin * zsin),
        .m10 = xsin * ycos, 
        .m11 = (xsin * ysin * zsin) + (xcos * zcos), 
        .m12 = (xsin * ysin * zcos) - (xcos * zsin),
        .m20 = -ysin, 
        .m21 = ycos * zsin, 
        .m22 = ycos * zcos,
    };

    return rot;
}

static inline void rotation_rotate_abs(rotation_t *rot, vec3 rotation) {
    *rot = rotation_create(rotation);
}

static inline void rotation_rotate_rel(rotation_t *rot, vec3 rotation) {
    *rot = rotation_create(itrn_v4f(rotation._itrn + rot->angles._itrn));
}

#endif
