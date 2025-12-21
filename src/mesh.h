#ifndef MESH_H
#define MESH_H

#include "math3D.h"
#include "cvector.h"

typedef struct Mesh
{
    cvector_vector_type(V3D) verts;
    cvector_vector_type(TRI) faces;
    cvector_vector_type(V3D) verts_transformed;
} Mesh;

extern Mesh g_Mesh;

int LoadObj(char* filename);
void FreeMesh(void);

#endif // MESH_H

