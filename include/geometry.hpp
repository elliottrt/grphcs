#ifndef _GRPHCS_GEOMETRY_H
#define _GRPHCS_GEOMETRY_H

#include "defs.hpp"
#include "material.hpp"
#include "graphics.hpp"

enum GrphcsGeometryType {
    NONE = 0,
    RECT,
    SPHERE,
    PLANE,
    CUSTOM
};

struct GrphcsTri {
    GrphcsVec normal;
    grphcs_tri_id_t v1, v2, v3;
    grphcs_material_id_t material;
};

class GrphcsGeometry {
    friend class GrphcsContext;

public:

    static GrphcsGeometry * fromID(grphcs_geometry_id_t id);

    static GrphcsGeometry * rectangle(GrphcsVec dimensions);
    static GrphcsGeometry * sphere(int slices, int stacks, float radius);
    static GrphcsGeometry * plane(int subdivisions, float width, float depth);

    GrphcsGeometry(char const * filepath);
    ~GrphcsGeometry(void);

    void destroy(void);

    void setScale(GrphcsVec scale);
    inline void setScale(float x, float y, float z) { this->setScale(GrphcsVec(x, y, z)); }
    void setRotation(GrphcsVec rotation);
    inline void setRotation(float x, float y, float z) { this->setRotation(GrphcsVec(x, y, z)); }

    inline void setScaleRelative(GrphcsVec scale) { this->setScale(this->scale_v + scale); }
    inline void setScaleRelative(float x, float y, float z) { this->setScaleRelative(GrphcsVec(x, y, z)); }
    inline void setRotationRelative(GrphcsVec rotation) { this->setRotation(this->rotation_v + rotation); }
    inline void setRotationRelative(float x, float y, float z) { this->setRotationRelative(GrphcsVec(x, y, z)); }

    inline GrphcsVec getScale(void) { return this->scale_v; }
    inline GrphcsVec getRotation(void) { return this->rotation_v; }

    inline grphcs_geometry_id_t getID(void) { return this->id; }

    void print(void);
    size_t size(void);

private:

    std::vector<GrphcsVec> vertices;
    std::vector<GrphcsTri> triangles;

    GrphcsVec rotation_v, scale_v;
    GrphcsMat rotation_m, scale_m;
    GrphcsMat cache_translation_m;

    GrphcsMaterialList materials;

    grphcs_geometry_id_t id;

    GrphcsGeometryType type;

    GrphcsGeometry(GrphcsGeometryType type);

    void recalculateTranslation(void);

    void addTriangle(grphcs_tri_id_t a, grphcs_tri_id_t b, grphcs_tri_id_t c);
    inline void addVertex(float a, float b, float c) { this->vertices.push_back(GrphcsVec(a, b, c)); }
    inline void addVertex(GrphcsVec v) { this->vertices.push_back(v); }

    void loadFromFile(char const * filepath);

    // Geometry Loading Functions
    void parseLine(char * line);
    void parseVertex(char * line);
    void parseFace(char * line);
    void parseMtllib(char * line);
    void parseUsemtl(char * line);

};

#endif
