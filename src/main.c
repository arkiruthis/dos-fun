#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef __DJGPP__
#include <sys/nearptr.h>
#include <pc.h>
#endif

#include "utils.h"
#include "math3d.h"

#define WIDTH 320
#define HEIGHT 200
#define SIZE (WIDTH * HEIGHT)

static unsigned char back[SIZE];

static V4D cubeVerts[8] = {
    {-128, -128, 128, 1},   // FRONT TOP LEFT
    {128, -128, 128, 1},    // FRONT TOP RIGHT
    {128, 128, 128, 1},     // FRONT BOTTOM RIGHT
    {-128, 128, 128, 1},    // FRONT BOTTOM LEFT
    {-128, -128, -128, 15}, // BACK TOP LEFT
    {128, -128, -128, 15},  // BACK TOP RIGHT
    {128, 128, -128, 15},   // BACK BOTTOM RIGHT
    {-128, 128, -128, 15},  // BACK BOTTOM LEFT
};

// Clockwise winding
static int triList[12 * 3] = {
    0, 1, 2, // FR 1
    0, 2, 3, // FR 2
    5, 4, 7, // BK 1
    5, 7, 6, // BK 2
    4, 0, 3, // LT 1
    4, 3, 7, // LT 2
    1, 5, 6, // RT 1
    1, 6, 2, // RT 2
    3, 2, 6, // BT 1
    3, 6, 7, // BT 2
    4, 5, 1, // TP 1
    4, 1, 0  // TP 2
};

void hline(int length, fix c1, fix c2, unsigned char *ptr)
{
  fix xstep = (((c2 - c1) >> 8) + 1) * oneover(length);

  do
  {
    *ptr++ = (c1 >> 8);
    c1 += xstep;
  } while (length-- > 0);
}

void draw_tris(V4D *verts, int triList[])
{
  fix i, j, k;
  fix short_dx, long_dx, lx, rx;
  fix long_cx, short_cx, lc, rc;
  V4D a, b, c;
  unsigned char *ptr, *ptrEnd;

  for (i = 0; i < 12; ++i)
  {
    a = verts[triList[(i * 3) + 0]];
    b = verts[triList[(i * 3) + 1]];
    c = verts[triList[(i * 3) + 2]];

    // Shifting by 11 gets 65536 down to 64 which fits okay as a max 128 within 200 height
    a.x = (a.x >> 2) + (WIDTH >> 1);
    a.y = (a.y >> 2) + (HEIGHT >> 1);
    a.z = min(max(24 - (a.z >> 4), 0), 63);
    b.x = (b.x >> 2) + (WIDTH >> 1);
    b.y = (b.y >> 2) + (HEIGHT >> 1);
    b.z = min(max(24 - (b.z >> 4), 0), 63);
    c.x = (c.x >> 2) + (WIDTH >> 1);
    c.y = (c.y >> 2) + (HEIGHT >> 1);
    c.z = min(max(24 - (c.z >> 4), 0), 63);

    if (orient2dint(a, b, c) > 0)
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
      j = a.z;
      a.z = b.z;
      b.z = j;
    }
    if (a.y > c.y)
    {
      j = a.y;
      a.y = c.y;
      c.y = j;
      j = a.x;
      a.x = c.x;
      c.x = j;
      j = a.z;
      a.z = c.z;
      c.z = j;
    }
    if (b.y > c.y)
    {
      j = b.y;
      b.y = c.y;
      c.y = j;
      j = b.x;
      b.x = c.x;
      c.x = j;
      j = b.z;
      b.z = c.z;
      c.z = j;
    }

    int shortHeight = b.y - a.y;
    int longHeight = c.y - a.y;

    if (longHeight <= 0)
      continue;

    long_dx = (c.x - a.x) * oneover(longHeight);
    short_dx = (b.x - a.x) * oneover(shortHeight);
    lx = (a.x << 8);
    rx = (a.x << 8);
    long_cx = (c.z - a.z) * oneover(longHeight);
    short_cx = (b.z - a.z) * oneover(shortHeight);
    lc = (a.z << 8);
    rc = (a.z << 8);
    ptr = &back[a.y * WIDTH];

    if (shortHeight > 0) // Top Half
    {
      if (long_dx < short_dx)
      { // Left side long edge
        do
        {
          j = ((rx - lx) >> 8);
          ptrEnd = ptr + (lx >> 8);
          hline(j, lc, rc, ptrEnd);

          lx += long_dx;
          rx += short_dx;
          lc += long_cx;
          rc += short_cx;
          ptr += WIDTH;
        } while (--shortHeight > 0);
      }
      else
      { // Right side long edge
        do
        {
          j = ((rx - lx) >> 8);
          ptrEnd = ptr + (lx >> 8);
          hline(j, lc, rc, ptrEnd);

          rx += long_dx;
          lx += short_dx;
          rc += long_cx;
          lc += short_cx;
          ptr += WIDTH;
        } while (--shortHeight > 0);
      }
    }

    // Bottom Half

    shortHeight = c.y - b.y;
    if (shortHeight > 0)
    {
      short_dx = (c.x - b.x) * oneover(shortHeight);
      short_cx = (c.z - b.z) * oneover(shortHeight);

      if (short_dx < long_dx)
      { // Left side long edge
        rx = (b.x << 8);
        rc = (b.z << 8);

        do
        {
          j = ((rx - lx) >> 8);
          ptrEnd = ptr + (lx >> 8);
          hline(j, lc, rc, ptrEnd);

          lx += long_dx;
          rx += short_dx;
          lc += long_cx;
          rc += short_cx;
          ptr += WIDTH;
        } while (--shortHeight > 0);
      }
      else
      { // Right side long edge
        lx = (b.x << 8);
        lc = (b.z << 8);

        do
        {
          j = ((rx - lx) >> 8);
          ptrEnd = ptr + (lx >> 8);
          hline(j, lc, rc, ptrEnd);

          rx += long_dx;
          lx += short_dx;
          rc += long_cx;
          lc += short_cx;
          ptr += WIDTH;
        } while (--shortHeight > 0);
      }
    }
  }
}

int main(void)
{
  unsigned char *vga;
  unsigned char *ptr;
  int i, x, y;
  int t = 0;
  time_t current_time;

  V4D cubeTransformed[8];
  MAT43 mat = {0};

  SetupTables();

// Map physical 0xA0000 into our flat address space
#ifdef __DJGPP__
  __djgpp_nearptr_enable();
  vga = (unsigned char *)(__djgpp_conventional_base + 0xA0000);
#else // Watcom C/C++
  vga = (unsigned char *)0xA0000;
#endif

  // Set VGA mode 13h
  set_video_mode(0x13);

  outportb(0x03C6, 0xFF); // Write mask
  for (i = 0; i < 64; ++i)
  {
    outportb(0x03C8, i);               // Color index
    outportb(0x03C9, min(i >> 1, 63)); // Red
    outportb(0x03C9, min(i, 63));      // Green
    outportb(0x03C9, min(i >> 2, 63)); // Blue
  }

  current_time = time(NULL);

  // Simple animation loop
  while (!kbhit())
  {
    EulerToMat(&mat, 256 + t, 55 + t, 77 + (t >> 2));
    // EulerToMat(&mat, 0, 0, 0);

    memset(&back[0], 0, SIZE);

    for (i = 0; i < 8; ++i)
    {
      MultV4DMatC(&cubeVerts[i], &cubeTransformed[i], &mat);
    }

    draw_tris(cubeTransformed, triList);

    // Blit back buffer -> VGA in one go
    memcpy(vga, &back[0], SIZE);
    ++t;
  }
  current_time = time(NULL) - current_time;

  getch();

  // Back to text mode 3
  set_video_mode(0x03);

  printf("Returned to text mode. Program finished.\n");
  printf("Elapsed time: %ld seconds\n", current_time);
  printf("Frame rate: %.2f FPS\n", 1000.0 / (double)current_time);

  return 0;
}
