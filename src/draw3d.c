#include "include/draw3d.h"
#include "include/game.h"
#include "include/math.h"

#define project_point(p) (p).x *= _far / (p).z; (p).y *= _far / (p).z;
#define point_not_in_view(p) ((p).z < _near || (p).z > _far)

/*
TODO:
3. optimize more
4. make fast - matrix -> float vecs?
3. Camera movement - find new projection thingy because close things explode
4. Mesh system - each mesh has a pointer to its geometry,
                rotation and scale matrix, and position
4. add (optional) shading to geometries
7. Use a format like Doxygen's to document all the code for readability
8. Put on github
10. Textures
4. Shaders - use function pointers that take in specific things and return certain things

*/

// drawing settings
static float _far = 100.f;
static float _near = 1.f;
static DrawMode mode = DM_ORTHOGRAPHIC;

// Object Picking and Depth Buffers
static PICKBUFFER_TYPE *pickbuf = NULL;
static float *depthbuf = NULL;
static size_t buf_size = 0;

/*
// rotation xyz
static float xrot, yrot, zrot;
// rotation matrix
static mat4 rotm = {
    1, 0, 0, 0,
    0, 1, 0, 0, 
    0, 0, 1, 0,
    0, 0, 0, 1
};
*/
// global rotations
static rotation_t g_rotation = {MAT4_IDENTITY, v3f(0, 0, 0)};
// scaling matrix
static mat4 sclm = MAT4_IDENTITY;

void zero_buffers() {
    // set all the floats to the farthest away something can be
    for (size_t i = 0; i < buf_size; i++) {
        depthbuf[i] = _far;
        pickbuf[i] = -1;
    }
}

static bool _pddbwarn = false;
void draw_depthbuf() {
    int max_depth = _far;
    if (!_pddbwarn) {
        _pddbwarn = true;
        fprintf(stderr, "draw_depthbuf() is very slow! Use at your own risk!\n");
    }
    for (int y = 0; y < game.ysize; y++) {
        for (int x = 0; x < game.xsize; x++) {
            uint8_t val = (depthbuf[(y * game.xsize) + x] / max_depth) * 255;
            draw_color(val, val, val, 255);
            SDL_RenderDrawPoint(window.renderer, x, y);
        }
    }
}

void color_from_id(PICKBUFFER_TYPE id) {
    uint q = (id * 214748) + 2592948 * (id % 4048);
    uint8_t r = q & 0xFF;
    uint8_t g = (q >> 8) & 0xFF;
    uint8_t b = (q >> 16) & 0xFF;
    draw_color(r, g, b, 255);
}

static bool _pdpbwarn = false;
void draw_pickbuf() {
    if (!_pdpbwarn) {
        _pdpbwarn = true;
        fprintf(stderr, "draw_pickbuf() is very slow! Use at your own risk! It also doesn't work properly!\n");
    }
    for (int y = 0; y < game.ysize; y++) {
        for (int x = 0; x < game.xsize; x++) {
            if (pickbuf[y * game.xsize + x] != -1) {
                color_from_id(pickbuf[y * game.xsize + x]);
                SDL_RenderDrawPoint(window.renderer, x, y);
            }
        }
    }
}

PICKBUFFER_TYPE get_vertex(void) {
    if (
        game.mouse.x < 0 || 
        game.mouse.x >= game.xsize || 
        game.mouse.y < 0 || 
        game.mouse.y >= game.ysize
    ) return -1;
    return pickbuf[game.mouse.y * game.xsize + game.mouse.x];
}

static bool draw_setup_called = false;
void draw3d_init() {
    buf_size = game.xsize * game.ysize;
    depthbuf = (float *) calloc(buf_size, sizeof(float));
    pickbuf = (PICKBUFFER_TYPE *) calloc(buf_size, sizeof(PICKBUFFER_TYPE));
    if (depthbuf == NULL || pickbuf == NULL) {
        fprintf(stderr, "Error allocating depth buffer or picking buffer.\n");
        exit(1);
    } 
    zero_buffers();
    if (draw_setup_called == false) {
        fprintf(stderr, "Using 3D graphics. draw_setup(drawmode, farplane) not called. Defaulting to Orthographic.");
    }
}

int draw_setup(DrawMode drawmode, float farplane) {
    if ((drawmode != DM_ORTHOGRAPHIC && drawmode != DM_PERSPECTIVE) || farplane <= 0) return 1;
    mode = drawmode;
    _far = farplane;
    draw_setup_called = true;
    return 0;
}

// Simple Drawing ---------------

static PICKBUFFER_TYPE current_id = 0;
static inline void check_set_pixel(vec3 pos) {
    pos.x += game.xsize / 2;
    pos.y += game.ysize / 2;
    if (pos.x < 0 || pos.x >= game.xsize || pos.y < 0 || pos.y >= game.ysize) return;
    int offset = (int)pos.x + ((int)pos.y * game.xsize);
    if ((size_t)offset >= buf_size || offset < 0) return;
    if (pos.z < depthbuf[offset]) {
        depthbuf[offset] = pos.z;
        pickbuf[offset] = current_id;
        SDL_RenderDrawPoint(window.renderer, pos.x, pos.y);
    }
}

void point3d(vec3 p) {
    check_set_pixel(p);
}

static inline void point3df(int x, int y, float z) {
    check_set_pixel(v3f((float)x, (float)y, z));
}

// much faster code for drawing horizontal lines
void _hline(int y, int x0, int x1, float z0, float z1) {
    if (x1 < x0) {
        int tmp = x0;
        x0 = x1;
        x1 = tmp;
    }

    float z = z0;
    float zstep = (z1 - z0) / (x1 - x0);

    for (int x = x0; x < x1; x++) {
        point3df(x, y, z);
        z += zstep;
    }
}

// draws a line, assuming that
// the vectors it gets are already translated
// and projected
void _line3d(vec3 a, vec3 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;

    float x = a.x;
    float y = a.y;
    float z = a.z;

    int steps = (int) ceilf(fmaxf(fabsf(dx), fabsf(dy)));

    float xstep = dx / steps;
    float ystep = dy / steps;
    float zstep = (b.z - a.z) / steps;

    for (int i = 0; i <= steps; ++i) {
        point3df(x, y, z);
        x += xstep;
        y += ystep;
        z += zstep;
    }

}

static bool _pl3dwarn = false;
void line3d(vec3 a, vec3 b) {

    if (!_pl3dwarn) {
        fprintf(stderr, "DO NOT USE line3d(a, b) IN draw3d.c! USE _line3d(a, b) INSTEAD!\n");
        _pl3dwarn = true;
    }
    
    // get the middle of the line
    vec3 middle = v3avg2(a, b);
    
    // convert a and b into offsets from the middle
    a._itrn -= middle._itrn;
    b._itrn -= middle._itrn;

    // rotate and scale them - 
    // this allows rotating around the center
    a = m4mulv4(g_rotation.rotm, m4mulv4(sclm, a));
    b = m4mulv4(g_rotation.rotm, m4mulv4(sclm, b));

    // add the average back, returning
    // to the original points but
    // rotated around the middle
    a._itrn += middle._itrn;
    b._itrn += middle._itrn;

    // remove if outside of the camera bounds
    if (point_not_in_view(a) || point_not_in_view(b)) return;

    // project the points if
    // in perspective
    if (mode == DM_PERSPECTIVE) { 
        project_point(a);
        project_point(b); 
    }

    _line3d(a, b);
}

// Triangles --------------------

// utility for swapping vec3
static inline void swap_vec3(vec3 *a, vec3 *b) {
    vec3 tmp = *a;
    *a = *b;
    *b = tmp;
}

// flat bottomed triangle
void fill_flat_bot_tri(vec3 v1, vec3 v2, vec3 v3) {

    float v2t1 = v2.y - v1.y;
    if (fabsf(v2t1) < 1) v2t1 /= fabsf(v2t1);
    if (isnan(v2t1)) return;
    float v3t1 = v3.y - v1.y;
    if (fabsf(v3t1) < 1) v3t1 /= fabsf(v3t1);
    if (isnan(v3t1)) return;

    float islope1 = (v2.x - v1.x) / v2t1;
    float islope2 = (v3.x - v1.x) / v3t1;
    float cx1 = v1.x;
    float cx2 = v1.x;
    float zslope1 = (v2.z - v1.z) / v2t1;
    float zslope2 = (v3.z - v1.z) / v3t1;
    float cz1 = v1.z;
    float cz2 = v1.z;

    for (int scanlineY = v1.y; scanlineY < v2.y; scanlineY++)
    {
        _hline(scanlineY, cx1, cx2, cz1, cz2);
        cx1 += islope1;
        cx2 += islope2;
        cz1 += zslope1;
        cz2 += zslope2;
    }

}

// flat top triangle
void fill_flat_top_tri(vec3 v1, vec3 v2, vec3 v3) {

    float v3t1 = v3.y - v1.y;
    if (fabsf(v3t1) < 1) v3t1 /= fabsf(v3t1);
    if (isnan(v3t1)) return;
    float v3t2 = v3.y - v2.y;
    if (fabsf(v3t2) < 1) v3t2 /= fabsf(v3t2);
    if (isnan(v3t2)) return;

    float islope1 = (v3.x - v1.x) / v3t1;
    float islope2 = (v3.x - v2.x) / v3t2;
    float cx1 = v3.x;
    float cx2 = v3.x;
    float zslope1 = (v3.z - v1.z) / v3t1;
    float zslope2 = (v3.z - v2.z) / v3t2;
    float cz1 = v3.z;
    float cz2 = v3.z;

    for (int scanlineY = v3.y; scanlineY >= v1.y; scanlineY--)
    {
        // nan in here
        _hline(scanlineY, cx1, cx2, cz1, cz2);
        cx1 -= islope1;
        cx2 -= islope2;
        cz1 -= zslope1;
        cz2 -= zslope2;
    }

}

// http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void _triangle3d(vec3 a, vec3 b, vec3 c, bool fill) {

    // this can be moved down for performance
    // but it leaves lines on the edges
    _line3d(a, b);
    _line3d(b, c);
    _line3d(c, a);

    if (fill) {
        vec3 vecs[3] = {a, b, c};
        
        // sort vertices ascending by Y
        int midx;
        for (int i = 0; i < 2; i++) {
            midx = i;
            for (int j = i + 1; j < 3; j++)
                if (vecs[j].y < vecs[midx].y)
                    midx = j;
    
            swap_vec3(&vecs[midx], &vecs[i]);
        }

        if (vecs[1].y == vecs[2].y) {
            fill_flat_top_tri(vecs[0], vecs[1], vecs[2]);
        } else if (vecs[0].y == vecs[1].y) {
            fill_flat_bot_tri(vecs[0], vecs[1], vecs[2]);
        } else {
            vec3 v4 = v3f(
                vecs[0].x + ((vecs[1].y - vecs[0].y) / (vecs[2].y - vecs[0].y) * (vecs[2].x - vecs[0].x)), 
                vecs[1].y,
                vecs[0].z + ((vecs[1].y - vecs[0].y) / (vecs[2].y - vecs[0].y) * (vecs[2].z - vecs[0].z))
            );
            fill_flat_top_tri(vecs[1], v4, vecs[2]);
            fill_flat_bot_tri(vecs[0], vecs[1], v4);
        }
    }

}

static bool _pt3dwarn = false;
void triangle3d(vec3 a, vec3 b, vec3 c, bool fill) {

    if (!_pt3dwarn) {
        fprintf(stderr, "DO NOT USE triangle3d(a, b, c, fill) IN draw3d.c! USE _triangle3d(a, b, c) INSTEAD!\n");
        _pt3dwarn = true;
    }
    
    // get the middle of the line
    vec3 middle = v3avg3(a, b, c);
    
    // convert a and b into offsets from the middle
    a._itrn -= middle._itrn;
    b._itrn -= middle._itrn;
    c._itrn -= middle._itrn;

    // rotate and scale them - 
    // this allows rotating around the center
    a = m4mulv4(g_rotation.rotm, m4mulv4(sclm, a));
    b = m4mulv4(g_rotation.rotm, m4mulv4(sclm, b));
    c = m4mulv4(g_rotation.rotm, m4mulv4(sclm, c));

    // add the average back, returning
    // to the original points but
    // rotated around the middle
    a._itrn += middle._itrn;
    b._itrn += middle._itrn;
    c._itrn += middle._itrn;

    // remove if outside of the camera bounds
    if (point_not_in_view(a) || 
        point_not_in_view(b) || 
        point_not_in_view(c)) 
        return;

    // project the points if
    // in perspective
    if (mode == DM_PERSPECTIVE) { 
        project_point(a);
        project_point(b); 
        project_point(c); 
    }

    _triangle3d(a, b, c, fill);

}

// Geometry Drawing ---------------

// method that doesn't take into account the camera - some strangeness
bool simple_front_facing(vec3 normal) {
    return m4mulv4(g_rotation.rotm, normal).z > 0;
}

// method that does
bool camera_front_facing(vec3 normal, vec3 tri_vertex, vec3 pos) {
    vec3 rotated_normal = m4mulv4(g_rotation.rotm, normal);
    vec3 translated_vert = itrn_v3f(m4mulv4(g_rotation.rotm, m4mulv4(sclm, tri_vertex))._itrn + pos._itrn);
    vec3 N = itrn_v3f(translated_vert._itrn - game.camera.position._itrn);
    //return v3dotv3(rotated_normal, N) > 0;
    return v4dotv4(rotated_normal, N) < 0;
}

void draw_tri_normal(geo_t *geo, tri_t *tri, vec3 offset) {
    vec4 tri_center = itrn_v4f((vertex(geo, tri->v1)._itrn + vertex(geo, tri->v2)._itrn + vertex(geo, tri->v3)._itrn) / 3.f);
    vec4 normal_end = itrn_v4f(tri_center._itrn + tri->normal._itrn);

    tri_center = itrn_v4f(m4mulv4(g_rotation.rotm, tri_center)._itrn + offset._itrn);
    normal_end = itrn_v4f(m4mulv4(g_rotation.rotm, normal_end)._itrn + offset._itrn);

    if (point_not_in_view(tri_center) || point_not_in_view(normal_end)) return;
    if (mode == DM_PERSPECTIVE) {
        project_point(tri_center);
        project_point(normal_end);
    }
    if (camera_front_facing(tri->normal, vertex(geo, tri->v1), offset)) draw_color(255, 255, 255, 255);
    else draw_color(255, 0, 0, 255);
    _line3d(tri_center, normal_end);
}

int draw_geometry(geo_t *geo, vec3 pos, bool fill) {
    if (point_not_in_view(pos)) return 0;
    int i; tri_t tri;
    vec_foreach(&geo->tris, tri, i) {
        //if (i != 6170) {} else
        if (camera_front_facing(tri.normal, vertex(geo, tri.v1), pos)) {
            vec3 points[3] = {vertex(geo, tri.v1), vertex(geo, tri.v2), vertex(geo, tri.v3)};
            bool outside_render = false;
            for (int i = 0; i < 3; i++) {
                vec3 point = points[i];
                point = m4mulv4(g_rotation.rotm, m4mulv4(sclm, point));
                point._itrn += pos._itrn - game.camera.position._itrn;
                point = m4mulv4(game.camera.rotation.rotm, point);
                if (point_not_in_view(point)) {
                    outside_render = true;
                    break;
                }
                if (mode == DM_PERSPECTIVE) { project_point(point); }
                points[i] = point;
            }
            if (!outside_render) {
                draw_color(0, 255 - (i * 20), 127, 255);
                current_id = (PICKBUFFER_TYPE) i;
                _triangle3d(points[0], points[1], points[2], fill);
            }
        }
        //draw_tri_normal(geo, &tri, pos);
    }
    return 0;
}

void rotate(float x, float y, float z) {
    rotation_rotate_abs(&g_rotation, v3f(x, y, z));
}

void rotate_relative(float x, float y, float z) {
    rotation_rotate_rel(&g_rotation, v3f(x, y, z));
}

void scale(float x, float y, float z) {
    sclm.m00 = x;
    sclm.m11 = y;
    sclm.m22 = z;
}
