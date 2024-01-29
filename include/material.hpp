#ifndef _GRPHCS_MATERIAL_H
#define _GRPHCS_MATERIAL_H

#include "defs.hpp"

// See https://www.loc.gov/preservation/digital/formats/fdd/fdd000508.shtml#:~:text=An%20OBJ%20file%20refers%20to,definition%20with%20a%20usemtl%20statement

struct GrphcsMaterial {

    // TODO: consider replacing these with precalculated uint8_t
    // vectors to store color without floats and less space
    GrphcsPixel colorAmbient;
    GrphcsPixel colorDiffuse;
    GrphcsPixel colorSpecular;
    // TODO: do this
    // We don't like things that produce light right now
    //float emission_coeff[3];

    int illumination;

    float specular_highlight;
    float optical_density;
    float dissolve;

    //size_t diffuse_map_size;
    //uint8_t * diffuse_map;

    char * name;

};

class GrphcsMaterialList {

public:

    GrphcsMaterialList(void);

    void loadMaterialFile(char * filepath);

    grphcs_material_id_t findMaterial(char * name);

    void clear(void);
    inline size_t size(void) { return this->materials.size(); }

    GrphcsMaterial * operator[](int idx) {
        return this->materials[idx];
    }

private:

    std::vector<GrphcsMaterial *> materials;
    grphcs_material_id_t workingMaterial;

    void createMaterial(char * name);

    void parseFloats(float * flts, char * line, int expected_count, char const * expected_attr);
    void parseInts(int * ints, char * line, int expected_count, char const * expected_attr);
    void parseUint8s(uint8_t * ints, char * line, int expected_count, char const * expected_attr);

};

#endif
