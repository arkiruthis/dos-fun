#ifndef RENDER_H
#define RENDER_H

#include "types.h"

#define BACKBUFFER_WIDTH 320
#define BACKBUFFER_HEIGHT 200
#define BACKBUFFER_SIZE (BACKBUFFER_WIDTH * BACKBUFFER_HEIGHT)

extern unsigned char back[];

struct V4D;

void DrawTris(struct V4D *verts, int triList[]);

#endif // RENDER_H