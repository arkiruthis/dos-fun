#ifndef RENDER_H
#define RENDER_H

#include "types.h"

#include <stddef.h>

#define BACKBUFFER_WIDTH 320
#define WIDTH_LEFT_BOUNDARY (-BACKBUFFER_WIDTH / 2)
#define WIDTH_RIGHT_BOUNDARY (BACKBUFFER_WIDTH / 2)
#define BACKBUFFER_HEIGHT 200
#define WIDTH_TOP_BOUNDARY (-BACKBUFFER_HEIGHT / 2)
#define WIDTH_BOTTOM_BOUNDARY (BACKBUFFER_HEIGHT / 2)
#define BACKBUFFER_SIZE (BACKBUFFER_WIDTH * BACKBUFFER_HEIGHT)
#define RADIX_DEPTH 256

void DrawTris();
void BlitBackBufferToVGA();
void ClearRenderQueue();

#endif // RENDER_H
