#ifndef _GRPHCS_CONTEXT_H
#define _GRPHCS_CONTEXT_H

#include "defs.hpp"
#include "camera.hpp"

class GrphcsGeometry;

// TODO: move picking into the context
class GrphcsContext {

public:

    GrphcsCamera camera;

    GrphcsContext(int xSize, int ySize, float fov, float * depthb, GrphcsPixel * pixelb, grphcs_pickbuffer_t * pickb);
    GrphcsContext() = default;

    void drawPoint(GrphcsVec point, GrphcsPixel color);
    void drawLine(GrphcsVec p0, GrphcsVec p1, GrphcsPixel color);
    void drawTriangle(GrphcsVec p0, GrphcsVec p1, GrphcsVec p2, GrphcsPixel color, bool fill);
    void drawTriangleBlend(GrphcsVec p0, GrphcsVec p1, GrphcsVec p2, GrphcsPixel c0, GrphcsPixel c1, GrphcsPixel c2);
    void drawGeometry(GrphcsGeometry * geo, GrphcsVec point, bool fill);

    inline int getXSize(void) { return this->xSize; }
    inline int getYSize(void) { return this->ySize; }

    grphcs_tri_id_t pickFace(int x, int y);
    GrphcsGeometry * pickGeometry(int x, int y);

private:

    int bufferSize, xSize, ySize;
    float * depthbuffer;
    GrphcsPixel * pixelbuffer;
    grphcs_pickbuffer_t * pickbuffer;
#if USE_PICK_BUFFER
    grphcs_pickbuffer_id_t currentGeoID;
    grphcs_pickbuffer_id_t currentFaceID;
#endif
    GrphcsVec offset;

    void checkSetPixel(GrphcsVec pos, GrphcsPixel color);
    void _line(GrphcsVec p0, GrphcsVec p1, GrphcsPixel color);
    void _hline(int y, GrphcsVec xz, GrphcsPixel color);
    void _triangle(GrphcsVec p0, GrphcsVec p1, GrphcsVec p2, GrphcsPixel color, bool fill);
    void flatTopTriangle(GrphcsVec v1, GrphcsVec v2, GrphcsVec v3, GrphcsPixel color);
    void flatBottomTriangle(GrphcsVec v1, GrphcsVec v2, GrphcsVec v3, GrphcsPixel color);

    bool normalFrontFacing(GrphcsGeometry * geo, grphcs_tri_id_t id, GrphcsVec point);

    inline bool pointInView(GrphcsVec &point) {
        bool in_xbounds = point.x > 0 - this->xSize / 2 && point.x < this->xSize / 2;
        bool in_ybounds = point.y > 0 - this->ySize / 2 && point.y < this->ySize / 2;
        return point.z >= NEARPLANE && point.z < FARPLANE && in_xbounds && in_ybounds;
    }

};

#endif
