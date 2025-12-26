#ifndef RENDER_H
#define RENDER_H

#include "types.h"

#include <stddef.h>

#define BACKBUFFER_WIDTH 320
#define BACKBUFFER_HEIGHT 200
#define BACKBUFFER_SIZE (BACKBUFFER_WIDTH * BACKBUFFER_HEIGHT)
#define TRI_LIST_SIZE 256

struct V4D;
struct TRI;

// void SubmitTriangle(struct V4D *verts, struct TRI *renderQueue);
void DrawTris();
void BlitBackBufferToVGA();

#endif // RENDER_H