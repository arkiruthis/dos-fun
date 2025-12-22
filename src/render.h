#ifndef RENDER_H
#define RENDER_H

#include "types.h"

#include <stddef.h>

#define BACKBUFFER_WIDTH 320
#define BACKBUFFER_HEIGHT 200
#define BACKBUFFER_SIZE (BACKBUFFER_WIDTH * BACKBUFFER_HEIGHT)

extern unsigned char back[];

struct V4D;
struct TRI;

void DrawTris(struct V4D *verts, struct TRI *triList, size_t triCount);

#endif // RENDER_H