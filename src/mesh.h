#ifndef MESH_H
#define MESH_H

#include "math3d.h"
#include "cvector.h"

typedef struct Mesh
{
    cvector_vector_type(V4D) verts;
    cvector_vector_type(V4D) vertNormals;
    cvector_vector_type(TRI) faces;
    cvector_vector_type(V4D) vertsTransformed;
    cvector_vector_type(V4D) vertNormalsTransformed;
} Mesh;

extern Mesh g_Mesh;

int LoadObj(char* filename);
void FreeMesh(void);

#endif // MESH_H

