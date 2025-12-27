#ifndef RENDER_H
#define RENDER_H

#include "types.h"

#include <stddef.h>

#define BACKBUFFER_WIDTH 320
#define BACKBUFFER_HEIGHT 200
#define BACKBUFFER_SIZE (BACKBUFFER_WIDTH * BACKBUFFER_HEIGHT)
#define RADIX_DEPTH 256

void DrawTris();
void BlitBackBufferToVGA();

#endif // RENDER_H