#ifndef _GRPHCS_MATH_H
#define _GRPHCS_MATH_H

#define _GNU_SOURCE
#include <cmath>
#include <cassert>
#include <cstdio>

#include "util.hpp"

#define GRPHCS_MATH inline

#define DEG2RAD 0.017453
GRPHCS_MATH constexpr float deg2rad(float deg) { return deg * DEG2RAD; }
#define RAD2DEG 57.295780
GRPHCS_MATH constexpr float rad2deg(float rad) { return rad * RAD2DEG; }

struct GrphcsVec;

struct GrphcsMat {

    /*
    struct {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
    */
    float m[4][4] = {{1, 0, 0, 0},
                     {0, 1, 0, 0},
                     {0, 0, 1, 0},
                     {0, 0, 0, 1}};

    GrphcsMat(void) {}

    void print(void) const {
        /*
        printf("%f %f %f %f\n", this->m00, this->m01, this->m02, this->m03);
        printf("%f %f %f %f\n", this->m10, this->m11, this->m12, this->m13);
        printf("%f %f %f %f\n", this->m20, this->m21, this->m22, this->m23);
        printf("%f %f %f %f\n", this->m30, this->m31, this->m32, this->m33);
        */
        for (int i = 0; i < 4; i++) {
            printf("%f %f %f %f\n", this->m[i][0], this->m[i][1], this->m[i][2], this->m[i][3]);
        }
    }

    GrphcsMat quickInverse(void) const;

    GrphcsMat operator*(const GrphcsMat &o) const {
        GrphcsMat m;

        for (int c = 0; c < 4; c++) {
			for (int r = 0; r < 4; r++) {
                m.m[r][c] = this->m[r][0] * o.m[0][c] + 
                            this->m[r][1] * o.m[1][c] + 
                            this->m[r][2] * o.m[2][c] + 
                            this->m[r][3] * o.m[3][c];
            }
        }

        return m;
    }

    bool operator==(const GrphcsMat &o) const {
        /*
        return this->m00 == o.m00 && this->m01 == o.m01 && this->m02 == o.m02 && this->m03 == o.m03 &&
               this->m10 == o.m10 && this->m11 == o.m11 && this->m12 == o.m12 && this->m13 == o.m13 &&
               this->m20 == o.m20 && this->m21 == o.m21 && this->m22 == o.m22 && this->m23 == o.m23 &&
               this->m30 == o.m30 && this->m31 == o.m31 && this->m32 == o.m32 && this->m33 == o.m33;
        */

        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++){
                if (this->m[r][c] != o.m[r][c]) return false;
            }
        }
        return true;
    }

    // See https://github.com/BSVino/MathForGameDevelopers/blob/e5a9d5e4a6777b278b704f9c363d599f84186fd4/math/matrix.cpp
    // See https://github.com/C-Chads/tinygl/blob/e71df3beac500cfe5e974c6c1f33bd4883989fdc/include-demo/3dMath.h
    static GrphcsMat makeProjectionMatrix(float fov, float aspect, float near, float far);

    // See https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_olcEngine3D_Part3.cpp
    static GrphcsMat pointAt(const GrphcsVec &pos, const GrphcsVec &target, const GrphcsVec &up);

    // See https://www.3dgep.com/understanding-the-view-matrix/
    static GrphcsMat lookAt(const GrphcsVec &pos, const GrphcsVec &target, const GrphcsVec &up);

    // See https://www.3dgep.com/understanding-the-view-matrix/
    static GrphcsMat fpsView(const GrphcsVec &pos, float pitch, float yaw);

};

struct GrphcsVec {

    float x, y, z, w;

    GrphcsVec(float x, float y, float z, float w) {
        this->x = x; this->y = y;
        this->z = z; this->w = w;
    }

    GrphcsVec(float x, float y, float z) {
        this->x = x; this->y = y;
        this->z = z; this->w = 0;
    }

    GrphcsVec(float x, float y) {
        this->x = x; this->y = y;
        this->z = 0; this->w = 0;
    }

    GrphcsVec(float x) {
        this->x = x; this->y = 0;
        this->z = 0; this->w = 0;
    }

    GrphcsVec(void) {
        this->x = 0; this->y = 0;
        this->z = 0; this->w = 0;
    }

    GRPHCS_MATH float dot(const GrphcsVec &b) const {
        return this->x * b.x + this->y * b.y + this->z * b.z + this->w * b.w;
    }

    GRPHCS_MATH float squareMagnitude(void) const {
        return this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w;
    }

    GRPHCS_MATH float magnitude(void) const {
        return sqrtf(this->squareMagnitude());
    }

    GRPHCS_MATH float sum(void) const {
        return this->x + this->y + this->z + this->w;
    }

    GRPHCS_MATH GrphcsVec cross(GrphcsVec &b) const {
        return GrphcsVec ( 
            this->y * b.z - this->z * b.y, 
            this->z * b.x - this->x * b.z, 
            this->x * b.y - this->y * b.x
        );
    }

    GRPHCS_MATH GrphcsVec normalized(void) const {
        float magnitude = this->magnitude();
        return GrphcsVec(
            x / magnitude,
            y / magnitude,
            z / magnitude,
            w / magnitude
        );
    } 

    GRPHCS_MATH void normalize(void) {
        float magnitude = this->magnitude();
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
        w /= magnitude;
    } 

    GRPHCS_MATH void print(void) {
        printf("<%f, %f, %f, %f>\n", this->x, this->y, this->z, this->w);
    }

    GrphcsMat getRotationMatrix(void) {
        float xsin, ysin, zsin;
        float xcos, ycos, zcos;

        // TOOD: find fastest way to do this

        // This switch of X and Z is intentional.
        // Check https://en.wikipedia.org/wiki/Rotation_matrix#In_three_dimensions
        // If you think this is wrong.
        __sincosf(this->z, &xsin, &xcos);
        __sincosf(this->y, &ysin, &ycos);
        __sincosf(this->x, &zsin, &zcos);

        GrphcsMat rotation = GrphcsMat();

        rotation.m[0][0] = xcos * ycos;
        rotation.m[0][1] = (xcos * ysin * zsin) - (xsin * zcos);
        rotation.m[0][2] = (xcos * ysin * zcos) + (xsin * zsin);
        rotation.m[1][0] = xsin * ycos;
        rotation.m[1][1] = (xsin * ysin * zsin) + (xcos * zcos);
        rotation.m[1][2] = (xsin * ysin * zcos) - (xcos * zsin);
        rotation.m[2][0] = -ysin;
        rotation.m[2][1] = ycos * zsin;
        rotation.m[2][2] = ycos * zcos;

        return rotation;
    }

    GrphcsVec operator-(const GrphcsVec &other) const {
        return GrphcsVec(
            x - other.x,
            y - other.y,
            z - other.z,
            w - other.w
        );
    }

    GrphcsVec operator+(const GrphcsVec &other) const {
        return GrphcsVec(
            x + other.x,
            y + other.y,
            z + other.z,
            w + other.w
        );
    }

    GrphcsVec operator/(float other) const {
        return GrphcsVec(
            x / other,
            y / other,
            z / other,
            w / other
        );
    }

    GrphcsVec operator*(float other) const {
        return GrphcsVec(
            x * other,
            y * other,
            z * other,
            w * other
        );
    }

    // TODO: try to optimize
    GrphcsVec operator*(const GrphcsMat &m) const {
        /*
        return GrphcsVec (
            this->x * m.m00 + this->y * m.m10 + this->z * m.m20 + this->w * m.m30,
            this->x * m.m01 + this->y * m.m11 + this->z * m.m21 + this->w * m.m31,
            this->x * m.m02 + this->y * m.m12 + this->z * m.m22 + this->w * m.m32,
            this->x * m.m03 + this->y * m.m13 + this->z * m.m23 + this->w * m.m33
        );
        */
        return GrphcsVec (
            this->x * m.m[0][0] + this->y * m.m[1][0] + this->z * m.m[2][0] + this->w * m.m[3][0],
            this->x * m.m[0][1] + this->y * m.m[1][1] + this->z * m.m[2][1] + this->w * m.m[3][1],
            this->x * m.m[0][2] + this->y * m.m[1][2] + this->z * m.m[2][2] + this->w * m.m[3][2],
            this->x * m.m[0][3] + this->y * m.m[1][3] + this->z * m.m[2][3] + this->w * m.m[3][3]
        );
    }

    void operator-=(const GrphcsVec &other) { 
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
    }

    void operator+=(const GrphcsVec  other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
    }

    void operator/=(float other) {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
    }

    void operator*=(float other) {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
    }

    void operator*=(const GrphcsMat &m) {
        *this = *this * m;
    }

    bool operator==(GrphcsVec & o) {
        return this->x == o.x &&
               this->y == o.y &&
               this->z == o.z &&
               this->w == o.w;
    }

};


#endif
