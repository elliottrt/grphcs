#include "../include/geometry.hpp"

static std::vector<GrphcsGeometry *> geometries;
static grphcs_material_id_t currentMaterial = -1;

GrphcsGeometry * GrphcsGeometry::fromID(grphcs_geometry_id_t id) { 
    return geometries[id]; 
}

GrphcsGeometry * GrphcsGeometry::rectangle(GrphcsVec dimensions) {

    GrphcsGeometry * geo = new GrphcsGeometry(GrphcsGeometryType::RECT);

    GrphcsVec h = dimensions / 2;

    geo->addVertex(-h.x, -h.y, -h.z);
    geo->addVertex( h.x, -h.y, -h.z);
    geo->addVertex( h.x,  h.y, -h.z);
    geo->addVertex(-h.x,  h.y, -h.z);
    geo->addVertex(-h.x, -h.y,  h.z);
    geo->addVertex( h.x, -h.y,  h.z);
    geo->addVertex( h.x,  h.y,  h.z);
    geo->addVertex(-h.x,  h.y,  h.z);

    // TODO: make a loop for this

    geo->addTriangle(3, 1, 0);
    geo->addTriangle(3, 2, 1);

    geo->addTriangle(4, 5, 7);
    geo->addTriangle(5, 6, 7);

    geo->addTriangle(7, 0, 4);
    geo->addTriangle(7, 3, 0);

    geo->addTriangle(2, 6, 1);
    geo->addTriangle(6, 5, 1);

    geo->addTriangle(0, 1, 4);
    geo->addTriangle(1, 5, 4);

    geo->addTriangle(7, 2, 3);
    geo->addTriangle(6, 2, 7);

    return geo;

}

GrphcsGeometry * GrphcsGeometry::sphere(int slices, int stacks, float radius) {

    GrphcsGeometry * geo = new GrphcsGeometry(GrphcsGeometryType::RECT);

    geo->addVertex(0, radius, 0);
    grphcs_tri_id_t v0 = 0;

    for (int i = 0; i < stacks - 1; i++) {
        float phi = M_PI * (i + 1) / stacks;
        for (int j = 0; j < slices; j++) {
            float theta = 2.0 * M_PI * j / slices;
            float x = sinf(phi) * cosf(theta);
            float y = cosf(phi);
            float z = sinf(phi) * sinf(theta);

            GrphcsVec v = GrphcsVec(x, y, z);

            geo->addVertex(v * radius);
        }
    }

    geo->addVertex(0, -radius, 0);
    grphcs_tri_id_t v1 = geo->vertices.size() - 1;

    for (int i = 0; i < slices; ++i) {
        int i0 = i + 1;
        int i1 = (i + 1) % slices + 1;
        geo->addTriangle(v0, i1, i0);
        i0 = i + slices * (stacks - 2) + 1;
        i1 = (i + 1) % slices + slices * (stacks - 2) + 1;
        geo->addTriangle(v1, i0, i1);
    }

    for (int j = 0; j < stacks - 2; j++) {
        int j0 = j * slices + 1;
        int j1 = (j + 1) * slices + 1;
        for (int i = 0; i < slices; i++) {
            int i0 = j0 + i;
            int i1 = j0 + (i + 1) % slices;
            int i2 = j1 + (i + 1) % slices;
            int i3 = j1 + i;

            geo->addTriangle(i3, i0, i1);
            geo->addTriangle(i2, i3, i1);
        }
    }

    return geo;

}

GrphcsGeometry * GrphcsGeometry::plane(int subdivisions, float width, float depth) {

    GrphcsGeometry * geo = new GrphcsGeometry(GrphcsGeometryType::PLANE);

    GrphcsVec v = GrphcsVec(0, 0, 0);
    for (int i = 0; i < subdivisions + 1; i++) {
        for (int j = 0; j < subdivisions + 1; j++) {
            geo->addVertex(v);
            v.z += depth / subdivisions;
        }
        v.z = 0;
        v.x += width / subdivisions;
    }

    for (int i = 0; i < subdivisions; i++) {
        for (int j = 0; j < subdivisions; j++) {
            grphcs_tri_id_t v0 = j + i * (subdivisions + 1);
            grphcs_tri_id_t v1 = v0 + subdivisions + 1;
            grphcs_tri_id_t v2 = v0 + subdivisions + 2;
            grphcs_tri_id_t v3 = v0 + 1;
            geo->addTriangle(v0, v1, v2);
            geo->addTriangle(v0, v2, v3);
        }
    }

    return geo;

}


GrphcsGeometry::GrphcsGeometry(char const * filepath) {

    this->loadFromFile(filepath);

    this->rotation_m = GrphcsMat();
    this->scale_m = GrphcsMat();

    this->rotation_v = GrphcsVec(0, 0, 0);
    this->scale_v = GrphcsVec(1, 1, 1);

    this->id = geometries.size();
    geometries.push_back(this);

    this->type = GrphcsGeometryType::CUSTOM;

}

GrphcsGeometry::GrphcsGeometry(GrphcsGeometryType type) {

    this->rotation_m = GrphcsMat();
    this->scale_m = GrphcsMat();

    this->rotation_v = GrphcsVec();
    this->scale_v = GrphcsVec();

    this->id = geometries.size();
    geometries.push_back(this);

    this->type = type;

}

GrphcsGeometry::~GrphcsGeometry(void) {
    this->destroy();
}

void GrphcsGeometry::destroy(void) {

    this->triangles.clear();
    this->vertices.clear();
    
    this->materials.clear();

    geometries.erase(geometries.begin() + this->id);

}

void GrphcsGeometry::setScale(GrphcsVec scale) {
    this->scale_v = scale;
    this->scale_m.m[0][0] = scale.x;
    this->scale_m.m[1][1] = scale.y;
    this->scale_m.m[2][2] = scale.z;
    this->scale_m.m[3][3] = scale.w;
}

void GrphcsGeometry::setRotation(GrphcsVec rotation) {

    this->rotation_v = rotation;
    this->rotation_m = rotation.getRotationMatrix();

}

void GrphcsGeometry::recalculateTranslation(void) {
    this->cache_translation_m = this->scale_m * this->rotation_m;
}

void GrphcsGeometry::addTriangle(grphcs_tri_id_t a, grphcs_tri_id_t b, grphcs_tri_id_t c) {
    GrphcsTri tri = GrphcsTri();

    tri.v1 = a; tri.v2 = b; tri.v3 = c;
    tri.material = currentMaterial;

    GrphcsVec u = (this->vertices[tri.v2] - this->vertices[tri.v1]).i;
    GrphcsVec v = (this->vertices[tri.v3] - this->vertices[tri.v1]).i;

    tri.normal = u.cross(v);
    tri.normal.normalize();

    this->triangles.push_back(tri);
}

void GrphcsGeometry::loadFromFile(char const * filepath) {

    this->triangles.clear();
    this->vertices.clear();

    FILE * file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open geometry file '%s': %s\n", filepath, strerror(errno));
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        if (read > 0) this->parseLine(line);
    }

    fclose(file);

    this->triangles.shrink_to_fit();
    this->vertices.shrink_to_fit();

}

void GrphcsGeometry::print(void) {
    printf("geometry = {\n");
    for (grphcs_tri_id_t id = 0; id < (grphcs_tri_id_t) this->triangles.size(); id++) {
        GrphcsTri tri = this->triangles[id];
        printf("\tTri_%i = (%i, %i, %i), %i,\n", id, tri.v1, tri.v2, tri.v3, tri.material);
    }
    printf("\n");
    for (grphcs_tri_id_t id = 0; id < (grphcs_tri_id_t) this->vertices.size(); id++) {
        GrphcsVec vert = this->vertices[id];
        printf("\tVert_%i = (%f, %f, %f),\n", id, vert.x, vert.y, vert.z);
    }
    printf("\n");
    for (grphcs_material_id_t id = 0; id < (grphcs_material_id_t) this->materials.size(); id++) {
        GrphcsMaterial * mat = this->materials[id];

        printf("\tMaterial_%i = {\n", id);

        printf("\t\tName = %s\n", mat->name);

        printf("\t\tAmbient Color = (%ui, %ui, %ui)\n", mat->colorAmbient.r, mat->colorAmbient.g, mat->colorAmbient.b);
        printf("\t\tDiffuse Color = (%ui, %ui, %ui)\n", mat->colorDiffuse.r, mat->colorDiffuse.g, mat->colorDiffuse.b);
        printf("\t\tSpecular Color = (%ui, %ui, %ui)\n", mat->colorSpecular.r, mat->colorSpecular.g, mat->colorSpecular.b);

        printf("\t\tIllumination = %i\n", mat->illumination );

        printf("\t\tSpecular Highlight = %f\n", mat->specular_highlight );
        printf("\t\tOptical Density = %f\n", mat->optical_density );
        printf("\t\tDissolve = %f\n", mat->dissolve );

        printf("\t}\n");
    }
    printf("}\n");
}

size_t GrphcsGeometry::size(void) {
    return this->vertices.size() * sizeof(GrphcsVec) +
           sizeof(this->vertices) +
           this->triangles.size() * sizeof(GrphcsTri) + 
           sizeof(this->triangles) +
           sizeof(this->rotation_v) + 
           sizeof(this->rotation_m) + 
           sizeof(this->scale_v) + 
           sizeof(this->scale_m) + 
           sizeof(this->cache_translation_m) + 
           this->materials.size() * sizeof(GrphcsMaterial) + 
           sizeof(this->materials) +
           sizeof(this->id) +
           sizeof(this->type);
}


// Geometry Loading Functions

void GrphcsGeometry::parseLine(char * line) {

    if (strlen(line) < 1) return;

    switch ( line[0] ) {
        case 'v': { // we only want a normal vertex
            if (line[1] == ' ') 
                this->parseVertex(line);
        } break;
        case 'f': { // face (tri) data
            this->parseFace(line); 
        } break;
        case 'u': { // usemtl
            this->parseUsemtl(line);
        } break;
        case 'm': { // mtllib
            this->parseMtllib(line);
        } break;
        default: return;
    }
    
}

void GrphcsGeometry::parseVertex(char * line) {
    char *section = strtok(line, " ");
    assert(section != NULL);
    float parts[4] = {0, 0, 0, 0};
    int i = 0;

    while(section != NULL)
	{
		if (section[0] != 'v') {
            parts[i++] = atof(section);
        }
		section = strtok(NULL, " ");
	}

    this->addVertex(parts[0], parts[1], parts[2]);
}

void GrphcsGeometry::parseFace(char * line) {
    char *section = strtok(line, " ");
    assert(section != NULL);
    int i = 0;
    int ids[4] = {0, 0, 0, -1};

    while(section != NULL)
	{
		if (section[0] != 'f') {
            ids[i++] = atoi(section) - 1;
        }
		section = strtok(NULL, " ");
	}

    this->addTriangle(ids[0], ids[1], ids[2]);
    // If the f line has four indexes, add a second triangle to make a quad 
    if (ids[3] != -1) this->addTriangle(ids[0], ids[2], ids[3]);
}

// TODO: consider erroring if we don't get mtllib
void GrphcsGeometry::parseMtllib(char * line) {
    char *section = strtok(line, " ");
    assert(section != NULL);
    if ( strcmp(section, "mtllib") != 0 ) return;
    section = strtok(NULL, " ");
    assert(section != NULL);
    this->materials.loadMaterialFile(section);
}

void GrphcsGeometry::parseUsemtl(char * line) {
    char *section = strtok(line, " ");
    assert(section != NULL);
    if (strcmp(section, "usemtl") != 0) return;
    section = strtok(NULL, " ");
    assert(section != NULL);

    currentMaterial = this->materials.findMaterial(section);
}
