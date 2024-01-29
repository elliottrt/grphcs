#ifndef _GRPHCS_CAMERA_H
#define _GRPHCS_CAMERA_H

#include "math.hpp"
#include "defs.hpp"
#include "util.hpp"

class GrphcsCamera {

public:

    // TODO: only generateViewMatrix right before we render a frame

    GrphcsCamera(float fov, int xSize, int ySize);
    GrphcsCamera() = default;

    void setRotation(GrphcsVec rotation);
    inline void setRotationRelative(GrphcsVec rotation) { this->setRotation(this->rotation_v + rotation); }

    inline void setPosition(GrphcsVec position) { this->position = position; this->generateViewMatrix(); }
    inline void setPositionRelative(GrphcsVec position) { this->position += position; }

    inline void projectPoint(GrphcsVec &point) {

        //point *= this->view_m;

        point *= this->perspective_m;

        point /= point.w;
        point.x = -point.x;
        point.y = -point.y;
        point.x *= this->xSize / 2.f;
        point.y *= this->ySize / 2.f;

        //point.print();

        //point.x *= FARPLANE / point.z;
        //point.y *= FARPLANE / point.z;
    }

    inline bool pointInView(GrphcsVec &point) {
        // TODO: implement me
        UNUSED(point);
        return false;
    }

    void generateViewMatrix(void);

private:

    GrphcsVec position;
    GrphcsVec rotation_v;
    GrphcsMat rotation_m;
    GrphcsMat view_m;
    GrphcsMat perspective_m;

    int xSize, ySize;

};

#endif
