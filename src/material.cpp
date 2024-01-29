#include "../include/material.hpp"

GrphcsMaterialList::GrphcsMaterialList(void) {}

void GrphcsMaterialList::loadMaterialFile(char * filepath) {

    this->clear();

    char * lastchar = filepath + strlen(filepath) - 1;
    if (*lastchar == 10) *lastchar = 0;

    FILE * mtl_file = fopen(filepath, "r");
    if (mtl_file == NULL) {
        fprintf(stderr, "Failed to open material file '%s': %s\n", filepath, strerror(errno));
        return;
    }

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, mtl_file)) != -1) {
        if (read == 0) continue;

        GrphcsMaterial * mat = NULL;
        if ( this->workingMaterial >= 0 ) mat = this->materials[this->workingMaterial];

        switch ( *line ) {
            case 'n': { // newmtl 
                char *section = strtok(line, " ");
                assert(section != NULL);
                if ( strcmp(section, "newmtl") != 0 ) return;
                section = strtok(NULL, " ");
                assert(section != NULL);
                this->createMaterial(section);
            } break;
            case 'N': { // Optical Density or Specular Highlight
                if ( line[1] == 's' ) this->parseFloats( &mat->specular_highlight, line, 1, "Ns" );
                if ( line[1] == 'i' ) this->parseFloats( &mat->optical_density, line, 1, "Ni" );
            } break;
            case 'K': { // Ambient, Specular, or Diffuse Color
                if ( line[1] == 'a' ) this->parseUint8s((uint8_t*) &mat->colorAmbient, line, 3, "Ka");
                if ( line[1] == 'd' ) this->parseUint8s((uint8_t*) &mat->colorDiffuse, line, 3, "Kd");
                if ( line[1] == 's' ) this->parseUint8s((uint8_t*) &mat->colorSpecular, line, 3, "Ks");
            } break;
            case 'd': { // Dissolve
                this->parseFloats( &mat->dissolve, line, 1, "d" );
            } break;
            case 'i': { // Illumination Model
                this->parseInts( &mat->illumination, line, 1, "illum" );
            } break;
            /*
            case 'm': { // Diffuse Map
                material_process_map( mat, line );
            } break;
            */
            default: continue;
        }
    }
    fclose(mtl_file);
    this->materials.shrink_to_fit();
}

grphcs_material_id_t GrphcsMaterialList::findMaterial(char * name) {
    for (grphcs_material_id_t id = 0; id < (grphcs_material_id_t) this->materials.size(); id++) {
        if (strcmp(name, this->materials[id]->name) == 0) {
            return id;
        }
    }
    return -1;
}

void GrphcsMaterialList::clear(void) {

    this->workingMaterial = -1;

    for (grphcs_material_id_t id = 0; id < (grphcs_material_id_t) this->materials.size(); id++) {
        free(this->materials[id]->name);
        free(this->materials[id]);
    }

    this->materials.clear();

}

void GrphcsMaterialList::createMaterial(char * name) {
    GrphcsMaterial * mat = (GrphcsMaterial *) malloc(sizeof(GrphcsMaterial));

    mat->name = (char *) malloc(strlen(name) * sizeof(char));
    strcpy(mat->name, name);

    this->workingMaterial = this->materials.size();

    this->materials.push_back(mat);
}

#define PREPARE_PARSE(L, E) \
    char *section = strtok((L), " "); \
    assert(section != NULL); \
    if (strcmp(section, (E)) != 0) { \
        fprintf(stderr, "Error processing .obj file. Got '%s', expected '%s'.\n", section, (E)); \
        exit(1); \
    }

void GrphcsMaterialList::parseFloats(float * flts, char * line, int expected_count, char const * expected_attr) {
    PREPARE_PARSE(line, expected_attr);

    for ( int i = 0; i < expected_count; i++ ) {
        section = strtok(NULL, " ");
        assert(section != NULL);
        flts[i] = atof(section);
    }
}

void GrphcsMaterialList::parseInts(int * ints, char * line, int expected_count, char const * expected_attr) {
    PREPARE_PARSE(line, expected_attr);

    for ( int i = 0; i < expected_count; i++ ) {
        section = strtok(NULL, " ");
        assert(section != NULL);
        ints[i] = atoi(section);
    }
}

void GrphcsMaterialList::parseUint8s(uint8_t * ints, char * line, int expected_count, char const * expected_attr) {
    PREPARE_PARSE(line, expected_attr);

    for ( int i = expected_count - 1; i >= 0; i-- ) {
        section = strtok(NULL, " ");
        assert(section != NULL);
        ints[i] = (uint8_t) (atof(section) * 256) % 256;
    }
}
