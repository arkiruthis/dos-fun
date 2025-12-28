#include "render.h"
#include "math3d.h"
#include "mesh.h"

#ifdef __DJGPP__
#include <sys/nearptr.h>
#include <pc.h>
#endif

static unsigned char backBuffer[BACKBUFFER_SIZE];
static TRI *renderQueue[RADIX_DEPTH];

static inline void hline(int length, fix c1, fix c2, unsigned char *ptr)
{
  fix xstep = ((c2 - c1) * oneover(length)) >> 8;

  while (length-- > 0)
  {
    *ptr++ = c1 >> 8;
    c1 += xstep;
  }
}

void DrawTris()
{
  fix i, j, k;
  fix short_dx, long_dx, lx, rx;
  fix long_cx, short_cx, lc, rc;
  fix shortHeight, longHeight;
  V4D a, b, c;
  TRI *tri = NULL;
  unsigned char *ptr, *ptrEnd;

  // Draw a sky gradient, somewhat reminiscent of F-117 or other 90s DOS flight sims. :)
  ptr = backBuffer + BACKBUFFER_WIDTH * 149;
  lc = 87 << 8;
  k = 55;
  for (i = 0; i < 149; ++i)
  {
    memset(ptr, (unsigned char)(lc >> 8), BACKBUFFER_WIDTH);
    lc -= k;
    k = max(0, k - 1);
    ptr -= BACKBUFFER_WIDTH;
  }

  // Horizon to ground gradient
  ptr = backBuffer + BACKBUFFER_WIDTH * 150;
  lc = 88 << 8;
  k = 128;
  for (i = 0; i < 50; ++i)
  {
    memset(ptr, (unsigned char)(lc >> 8), BACKBUFFER_WIDTH);
    lc += k;
    k = max(0, k - 5);
    ptr += BACKBUFFER_WIDTH;
  }

  // First step is to go through our faces and, if CCW, add to our render queue.
  // We'll use the 'ol radix trick to sort them into buckets without qsort.

  for (i = 0; i < cvector_size(g_Mesh.faces); ++i)
  {
    tri = &g_Mesh.faces[i];

    a = g_Mesh.vertsTransformed[tri->a];
    b = g_Mesh.vertsTransformed[tri->b];
    c = g_Mesh.vertsTransformed[tri->c];
    j = tri->material_offset;
    k = (a.z + b.z + c.z); // aggregate depths
    k = clamp(127 + (k >> 3), 0, RADIX_DEPTH - 1);
    a.w += j;
    b.w += j;
    c.w += j;

    if (orient2dint(a, b, c) < 0)
      continue;

    // Sort vertices by Y
    if (a.y > b.y)
    {
      j = a.y;
      a.y = b.y;
      b.y = j;
      j = a.x;
      a.x = b.x;
      b.x = j;
      j = a.w;
      a.w = b.w;
      b.w = j;
    }
    if (a.y > c.y)
    {
      j = a.y;
      a.y = c.y;
      c.y = j;
      j = a.x;
      a.x = c.x;
      c.x = j;
      j = a.w;
      a.w = c.w;
      c.w = j;
    }
    if (b.y > c.y)
    {
      j = b.y;
      b.y = c.y;
      c.y = j;
      j = b.x;
      b.x = c.x;
      c.x = j;
      j = b.w;
      b.w = c.w;
      c.w = j;
    }

    longHeight = c.y - a.y;

    if (longHeight <= 0)
      continue;

    tri->v1 = (V2D){a.x, a.y};
    tri->v2 = (V2D){b.x, b.y};
    tri->v3 = (V2D){c.x, c.y};
    tri->c1 = a.w;
    tri->c2 = b.w;
    tri->c3 = c.w;

    // Add to the render queue
    tri->next = renderQueue[k];
    renderQueue[k] = tri;
  }

  for (i = RADIX_DEPTH - 1; i != 0; --i)
  {
    while (renderQueue[i])
    {
      tri = renderQueue[i];
      shortHeight = tri->v2.y - tri->v1.y;
      longHeight = tri->v3.y - tri->v1.y;

      long_dx = (tri->v3.x - tri->v1.x) * oneover16(longHeight);
      lx = (tri->v1.x << 16);
      rx = (tri->v1.x << 16);
      long_cx = (tri->c3 - tri->c1) * oneover(longHeight);
      lc = (tri->c1 << 8);
      rc = (tri->c1 << 8);
      ptr = backBuffer;

      ptr += (BACKBUFFER_SIZE + BACKBUFFER_WIDTH) >> 1; // Center horizontally
      ptr += (tri->v1.y * BACKBUFFER_WIDTH);

      if (shortHeight > 0) // Top Half
      {
        short_dx = (tri->v2.x - tri->v1.x) * oneover16(shortHeight);
        short_cx = (tri->c2 - tri->c1) * oneover(shortHeight);

        do
        {
          j = abs((rx >> 16) - (lx >> 16));
          k = min(lx, rx);
          ptrEnd = ptr + (k >> 16);
          if (lx <= rx)
            hline(j, lc, rc, ptrEnd);
          else
            hline(j, rc, lc, ptrEnd);

          lx += long_dx;
          rx += short_dx;
          lc += long_cx;
          rc += short_cx;
          ptr += BACKBUFFER_WIDTH;
        } while (--shortHeight > 0);
      }

      // Bottom Half

      shortHeight = tri->v3.y - tri->v2.y;
      if (shortHeight > 0)
      {
        short_dx = (tri->v3.x - tri->v2.x) * oneover16(shortHeight);
        short_cx = (tri->c3 - tri->c2) * oneover(shortHeight);

        rx = (tri->v2.x << 16);
        rc = (tri->c2 << 8);

        do
        {
          j = abs((rx >> 16) - (lx >> 16));
          k = min(lx, rx);
          ptrEnd = ptr + (k >> 16);
          if (lx <= rx)
            hline(j, lc, rc, ptrEnd);
          else
            hline(j, rc, lc, ptrEnd);

          lx += long_dx;
          rx += short_dx;
          lc += long_cx;
          rc += short_cx;
          ptr += BACKBUFFER_WIDTH;

        } while (--shortHeight > 0);
      }

      renderQueue[i] = tri->next;
    }
  }
}

void BlitBackBufferToVGA()
{
#ifdef __DJGPP__
  const unsigned char *vga = (unsigned char *)(__djgpp_conventional_base + 0xA0000);
#else // Watcom C/C++
  const unsigned char *vga = (unsigned char *)0xA0000;
#endif

  memcpy((void *)vga, (void *)backBuffer, BACKBUFFER_SIZE);
}

void ClearRenderQueue()
{
  memset(&renderQueue[0], 0, RADIX_DEPTH * sizeof(TRI *));
}