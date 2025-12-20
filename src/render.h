#ifndef RENDER_H
#define RENDER_H

#include "types.h"

#define BACKBUFFER_WIDTH 320
#define BACKBUFFER_HEIGHT 200
#define BACKBUFFER_SIZE (BACKBUFFER_WIDTH * BACKBUFFER_HEIGHT)

typedef struct EDGE
{
  unsigned char *ptr; // Pointer into backbuffer
  fix c;              // Current color/depth value
  fix xStep;          // Step per scanline
  fix span;           // Number of pixels to draw on this scanline
} EDGE;

extern EDGE g_edgeList[];
extern unsigned char back[];

#endif // RENDER_H