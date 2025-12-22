#include <stdio.h>
#include <string.h>
#include "mesh.h"
#include "cvector.h"

Mesh g_Mesh = {0};

void FreeMesh(void)
{
    cvector_free(g_Mesh.verts);
    cvector_free(g_Mesh.faces);
    cvector_free(g_Mesh.vertsTransformed);
    cvector_free(g_Mesh.vertNormals);
    cvector_free(g_Mesh.vertNormalsTransformed);
}

int LoadObj(char *filename)
{
    FILE *file;
    char line[256];
    int vertex_indices[3];
    int texture_indices[3]; // Unused currently
    int normal_indices[3];
    float vertex_float[3];
    int i;
    V4D vertex;
    V4D _verts[4];
    TRI face;

    g_Mesh.verts = NULL;
    g_Mesh.faces = NULL;
    g_Mesh.vertsTransformed = NULL;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("ERROR - Unable to find file.\n");
        return 1;
    }

    while (fgets(line, 256, file))
    {
        // Vertex information
        if (strncmp(line, "v ", 2) == 0)
        {
            sscanf(line, "v %f %f %f", &vertex_float[0], &vertex_float[1], &vertex_float[2]);
            vertex.x = float2fix(vertex_float[0]);
            vertex.y = float2fix(vertex_float[1]);
            vertex.z = float2fix(vertex_float[2]);
            cvector_push_back(g_Mesh.verts, vertex);
        }
        // Vertex Normal information
        if (strncmp(line, "vn ", 3) == 0)
        {
            sscanf(line, "vn %f %f %f", &vertex_float[0], &vertex_float[1], &vertex_float[2]);
            vertex.x = float2fix(vertex_float[0]);
            vertex.y = float2fix(vertex_float[1]);
            vertex.z = float2fix(vertex_float[2]);
            cvector_push_back(g_Mesh.vertNormals, vertex);
        }
        // // Face information
        if (strncmp(line, "f ", 2) == 0)
        {
            sscanf(
                line, "f %d//%d %d//%d %d//%d",
                &vertex_indices[0], &normal_indices[0],
                &vertex_indices[1], &normal_indices[1],
                &vertex_indices[2], &normal_indices[2]);

            face.a = vertex_indices[0] - 1;
            face.b = vertex_indices[1] - 1;
            face.c = vertex_indices[2] - 1;
            face.next = NULL;

            cvector_push_back(g_Mesh.faces, face);
        }
    }

    cvector_copy(g_Mesh.verts, g_Mesh.vertsTransformed);
    cvector_copy(g_Mesh.vertNormals, g_Mesh.vertNormalsTransformed);

    fclose(file);

    return 0;
}
