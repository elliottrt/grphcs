#include "../include/context.hpp"
#include "../include/geometry.hpp"

GrphcsContext::GrphcsContext(int xSize, int ySize, float fov, float * depthb, GrphcsPixel * pixelb, grphcs_pickbuffer_t * pickb) {
    this->camera = GrphcsCamera(fov, xSize, ySize);
    this->bufferSize = xSize * ySize;
    this->xSize = xSize;
    this->ySize = ySize;
    this->depthbuffer = depthb;
    this->pickbuffer = pickb;
    this->pixelbuffer = pixelb;
    this->offset = GrphcsVec(xSize / 2, ySize / 2);
}

void GrphcsContext::checkSetPixel( GrphcsVec pos, GrphcsPixel color ) {
    pos += this->offset;
    if (pos.x < 0 || pos.x >= this->xSize || pos.y < 0 || pos.y >= this->ySize) return;
    size_t offset = (int)pos.x + ((int)pos.y * this->xSize);

    if (pos.z < depthbuffer[offset]) {
        depthbuffer[offset] = pos.z;
#if USE_PICK_BUFFER
        pickbuffer[offset] = (((grphcs_pickbuffer_t) this->currentGeoID) << (sizeof(grphcs_pickbuffer_id_t) * 8)) | this->currentFaceID;
#endif
        pixelbuffer[ offset ] = color;
    }
}

void GrphcsContext::drawPoint(GrphcsVec point, GrphcsPixel color) {
    this->checkSetPixel(point, color);
}

void GrphcsContext::drawLine(GrphcsVec p0, GrphcsVec p1, GrphcsPixel color) {
    this->camera.projectPoint(p0);
    this->camera.projectPoint(p1);

    if (!this->pointInView(p0) || !this->pointInView(p1)) return;
    this->_line(p0, p1, color);
}

void GrphcsContext::drawTriangle(GrphcsVec p0, GrphcsVec p1, GrphcsVec p2, GrphcsPixel color, bool fill) {
    this->camera.projectPoint(p0);
    this->camera.projectPoint(p1); 
    this->camera.projectPoint(p2); 

    // remove if outside of the camera bounds
    if (this->pointInView(p0) && this->pointInView(p1) && this->pointInView(p2)) {
        this->_triangle(p0, p1, p2, color, fill);
    }
}

void GrphcsContext::drawTriangleBlend(GrphcsVec p0, GrphcsVec p1, GrphcsVec p2, GrphcsPixel c0, GrphcsPixel c1, GrphcsPixel c2) {
    this->camera.projectPoint(p0); 
    this->camera.projectPoint(p1); 
    this->camera.projectPoint(p2);

    if (this->pointInView(p0) && this->pointInView(p1) && this->pointInView(p2)) {
        
        GrphcsVec vecs[3] = { p0, p1, p2 };
        
        // sort vertices ascending by Y
        int midx;
        for (int i = 0; i < 2; i++) {
            midx = i;
            for (int j = i + 1; j < 3; j++)
                if (vecs[j].y < vecs[midx].y) midx = j;

            GrphcsVec tmp = vecs[midx];
            vecs[midx] = vecs[i];
            vecs[i] = tmp;
        }

        int t = vecs[0].y, 
            b = vecs[2].y, 
            r = fmaxf(fmaxf(p0.x, p1.x), p2.x), 
            l = fminf(fminf(p0.x, p1.x), p2.x);

        for (int y = t; y <= b; y++) {
            for (int x = l; x <= r; x++) {
                float w1 = ((p1.y - p2.y) * (x - p2.x) + (p2.x - p1.x) * (y - p2.y)) / ((p1.y - p2.y) * (p0.x - p2.x) + (p2.x - p1.x) * (p0.y - p2.y));
                float w2 = ((p2.y - p0.y) * (x - p2.x) + (p0.x - p2.x) * (y - p2.y)) / ((p1.y - p2.y) * (p0.x - p2.x) + (p2.x - p1.x) * (p0.y - p2.y));
                float w3 = 1 - w1 - w2;

                if ( w1 >= 0 && w2 >= 0 && w3 >= 0 ) {

                    float z = p0.z * w1 + p1.z * w2 + p2.z * w3;

                    GrphcsPixel color(
                        c0.r * w1 + c1.r * w2 + c2.r * w3,
                        c0.g * w1 + c1.g * w2 + c2.g * w3,
                        c0.b * w1 + c1.b * w2 + c2.b * w3
                    );

                    this->checkSetPixel(GrphcsVec(x, y, z), color);

                }
            }
        }
    }
}

void GrphcsContext::drawGeometry(GrphcsGeometry * geo, GrphcsVec point, bool fill) {
    //this->camera.projectPoint(point);
    //if (!this->pointInView(point)) return;

    geo->recalculateTranslation();

    for (grphcs_tri_id_t id = 0; id < (grphcs_tri_id_t) geo->triangles.size(); id++) {
        GrphcsTri tri = geo->triangles[id];
#if CULL_BACK_FACING
        if (this->normalFrontFacing(geo, id, point)) {
#endif
            GrphcsVec points[3] = {geo->vertices[tri.v1], geo->vertices[tri.v2], geo->vertices[tri.v3]};
            bool outside_render = false;

            // TODO: find a way to cache the transformed
            // points so we don't do as many calculations
            for (int i = 0; i < 3; i++) {
                GrphcsVec p = points[i];
                p *= geo->cache_translation_m;
                p += point;
                this->camera.projectPoint(p);
                if (!this->pointInView(p)) {
                    outside_render = true;
                    break;
                }
                points[i] = p;
            }
            
            if (!outside_render) {

                // TODO: diffuse supplies most of the color, but not all.
                // The other values in a material also affect color.

                //GrphcsPixel tri_color = NO_MATERIAL_COLOR;
                // TODO: change back and make some sort of system for
                // setting the materials of premades
                GrphcsPixel tri_color = GrphcsPixel(id % 256, (id * 20) % 256, (geo->id * 50) % 256);

                if ( tri.material >= 0 ) {
                    GrphcsMaterial * material = geo->materials[tri.material];

                    /*
                    Illumination Values:

                    0 : color = diffuse color
                    1 : color = KaIa + Kd { SUM j=1..ls, (N * Lj)Ij }
                    2: color = KaIa + Kd { SUM j=1..ls, (N*Lj)Ij } + Ks { SUM j=1..ls, ((H*Hj)^Ns)Ij }

                    Ia - ambient light, 
                    Ij - light j's intensity, 
                    Ka - ambient reflectance,
                    Kd - diffuse reflectance, 
                    Ks - specular reflectance, 
                    H -  unit vector bisector between L and V, 
                    L -  unit light vector, 
                    N -  unit surface normal, 
                    V -  unit view vector

                    */
                   // TODO: other illumination models
                    switch (material->illumination)
                    {
                        case 2:
                        case 1:
                        case 0: { // Just use diffuse color
                            tri_color = material->colorDiffuse;
                        } break;
                        // TODO: need to set this to 1 eventually
                        case 1000: { // lambertian shading
                            tri_color = material->colorAmbient * AMBIENT_LIGHT;
                            // this isn't right, we need to do light calculations, temporary
                            tri_color += material->colorDiffuse;

                        } break;
                        // TODO: need to set this to 2 eventually
                        case 2000: { // lambertian + blinn phong specular illumination
                            

                        } break;
                        default:
                            fprintf(stderr, "Invalid illumination of '%i' for material '%s'.\n", material->illumination, material->name);
                            exit(1);
                            break;
                    }

                }

#if USE_PICK_BUFFER
                this->currentFaceID = (grphcs_pickbuffer_id_t) id;
                this->currentGeoID = geo->id;
#endif
                this->_triangle(points[0], points[1], points[2], tri_color, fill);
            }
#if CULL_BACK_FACING
        }
#endif
    }
}

grphcs_tri_id_t GrphcsContext::pickFace(int x, int y) {
    if (x < 0 || x >= this->xSize || y < 0 || y >= this->ySize) 
        return -1;
    if (!USE_PICK_BUFFER)
        return -1;
    // We want the lower 32 bits, which contain the face information
    return this->pickbuffer[x + y * this->xSize] & 0xFFFFFFFF;
}

GrphcsGeometry * GrphcsContext::pickGeometry(int x, int y) {
    if (x < 0 || x >= this->xSize || y < 0 || y >= this->ySize) 
        return NULL;
    if (!USE_PICK_BUFFER)
        return NULL;
    // We want the upper 32 bits of information
    return GrphcsGeometry::fromID(this->pickbuffer[x + y * this->xSize] >> 32);
}


void GrphcsContext::_line(GrphcsVec p0, GrphcsVec p1, GrphcsPixel color) {
    GrphcsVec d = p1 - p0;

    int steps = (int) ceilf(fmaxf(fabsf(d.x), fabsf(d.y)));

    GrphcsVec curPoint = p0;
    GrphcsVec step = d / (float) steps;

    for (int i = 0; i <= steps; ++i) {
        this->checkSetPixel(curPoint, color);
        curPoint += step;
    }
}

void GrphcsContext::_hline(int y, GrphcsVec xz, GrphcsPixel color) {
    // xz contains the x and z positions of each side of the line -> <x1 x2 z1 z2>
    // TODO: figure out if we need to swap z1 and z2
    if (xz.y < xz.x) {
        float tmp = xz.x;
        xz.x = xz.y;
        xz.y = tmp;
        //tmp = xz.z;
        //xz.z = xz.w;
        //xz.w = tmp;
    }

    float z = xz.z;
    float zstep = (xz.w - xz.z) / (xz.y - xz.x);

    for (int x = xz.x; x < ceilf(xz.y); x++) {
        this->checkSetPixel( GrphcsVec(x, y, z) , color );
        z += zstep;
    }
}

void GrphcsContext::_triangle(GrphcsVec p0, GrphcsVec p1, GrphcsVec p2, GrphcsPixel color, bool fill) {
    if (fill) {
        GrphcsVec vecs[3] = {p0, p1, p2};
        
        // sort vertices ascending by Y
        int midx;
        for (int i = 0; i < 2; i++) {
            midx = i;
            for (int j = i + 1; j < 3; j++)
                if (vecs[j].y < vecs[midx].y)
                    midx = j;
    
            //swap(&vecs[midx], &vecs[i]);

            GrphcsVec tmp = vecs[midx];
            vecs[midx] = vecs[i];
            vecs[i] = tmp;
        }

        if (vecs[1].y == vecs[2].y) {
            this->flatBottomTriangle(vecs[0], vecs[1], vecs[2], color);
        } else if (vecs[0].y == vecs[1].y) {
            this->flatTopTriangle(vecs[0], vecs[1], vecs[2], color);
        } else {
            GrphcsVec values(vecs[1].y, vecs[2].y, vecs[2].x, vecs[2].z);
            GrphcsVec vals2(vecs[0].y, vecs[0].y, vecs[0].x, vecs[0].z);
            values -= vals2;

            GrphcsVec v4(
                vecs[0].x + (values.x / values.y * values.z), 
                vecs[1].y,
                vecs[0].z + (values.x / values.y * values.w)
            );

            this->flatTopTriangle(vecs[1], v4, vecs[2], color);
            this->flatBottomTriangle(vecs[0], vecs[1], v4, color);
        }
    } else {
        this->_line(p0, p1, color);
        this->_line(p1, p2, color);
        this->_line(p2, p0, color);
    }
}

void GrphcsContext::flatTopTriangle(GrphcsVec v1, GrphcsVec v2, GrphcsVec v3, GrphcsPixel color) {
    GrphcsVec d31 = v3 - v1;
    GrphcsVec d32 = v3 - v2;

    if (fabsf(d31.y) < 1) d31.y /= fabsf(d31.y);
    if (isnan(d31.y)) return;
    if (fabsf(d32.y) < 1) d32.y /= fabsf(d32.y);
    if (isnan(d32.y)) return;

    GrphcsVec numerators = { d31.x, d32.x, d31.z, d32.z };
    GrphcsVec denominators = { d31.y, d32.y, d31.y, d32.y };

    numerators.i /= denominators.i;

    GrphcsVec positions = { v3.x, v3.x, v3.z, v3.z };

    for (int scanlineY = floorf(v3.y); scanlineY >= ceilf(v1.y); scanlineY--)
    {
        this->_hline(scanlineY, positions, color);
        positions -= numerators;
    }
}

void GrphcsContext::flatBottomTriangle(GrphcsVec v1, GrphcsVec v2, GrphcsVec v3, GrphcsPixel color) {
    GrphcsVec d21 = v2 - v1;
    GrphcsVec d31 = v3 - v1;

    if (fabsf(d21.y) < 1) d21.y /= fabsf(d21.y);
    if (isnan(d21.y)) return;
    if (fabsf(d31.y) < 1) d31.y /= fabsf(d31.y);
    if (isnan(d31.y)) return;

    GrphcsVec numerators = GrphcsVec(d21.x, d31.x, d21.z, d31.z);
    GrphcsVec denominators = GrphcsVec(d21.y, d31.y, d21.y, d31.y);
    numerators.i /= denominators.i;

    GrphcsVec positions = { v1.x, v1.x, v1.z, v1.z };

    for (int scanlineY = floorf(v1.y); scanlineY < ceilf(v2.y); scanlineY++)
    {
        this->_hline(scanlineY, positions, color);
        positions += numerators;
    }
}

bool GrphcsContext::normalFrontFacing(GrphcsGeometry * geo, grphcs_tri_id_t id, GrphcsVec point) {
    GrphcsTri tri = geo->triangles[id];
    GrphcsVec vert = geo->vertices[tri.v1];

    GrphcsVec rotated_normal = tri.normal * geo->rotation_m;

    GrphcsVec translated_vert = (vert * geo->rotation_m) + point;
    
    // change inequality sign for different windings
    return rotated_normal.dot(translated_vert) < 0;
}
