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
static V3D cubeVerts[8] = {
    {-65536, -65536, 65536},  // FRONT TOP LEFT
    {65536, -65536, 65536},   // FRONT TOP RIGHT
    {65536, 65536, 65536},    // FRONT BOTTOM RIGHT
    {-65536, 65536, 65536},   // FRONT BOTTOM LEFT
    {-65536, -65536, -65536}, // BACK TOP LEFT
    {65536, -65536, -65536},  // BACK TOP RIGHT
    {65536, 65536, -65536},   // BACK BOTTOM RIGHT
    {-65536, 65536, -65536},  // BACK BOTTOM LEFT
};

// CW FRONT
static int triList[12 * 3] = {
    0, 1, 2,
    0, 2, 3,
    1, 5, 6,
    1, 6, 2,
    5, 4, 7,
    5, 7, 6,
    4, 0, 3,
    4, 3, 7,
    3, 2, 6,
    3, 6, 7,
    4, 5, 1,
    4, 1, 0};

void draw_line(fix x0, fix y0, fix x1, fix y1, unsigned char color)
{
  int i, j, height;
  fix dx, dy;
  unsigned char *ptr;

  x0 += int2fix(WIDTH / 2);
  y0 += int2fix(HEIGHT / 2);
  x1 += int2fix(WIDTH / 2);
  y1 += int2fix(HEIGHT / 2);

  dx = x1 - x0;
  height = (y1 - y0) >> 16;
  if (height == 0) // flat line
  {
    x0 >>= 16;
    x1 >>= 16;
    ptr = &back[(y0 >> 16) * WIDTH];
    ptr += (x0 < x1) ? x0 : x1;
    height = abs(dx >> 16);
    do
    {
      *ptr++ = color;
    } while (height-- > 0);
  }
  else
  {
    dx = fixdiv(dx, (y1 - y0));
    ptr = &back[(y0 >> 16) * WIDTH];
    do
    {
      ptr += (x0 >> 16);
      *ptr = color;
      ptr += WIDTH - (x0 >> 16);
      x0 += dx;
    } while (height-- > 0);
  }
}

void draw_tris(V3D *verts, int triList[], unsigned char color)
{
  fix i, j, k;
  fix short_dx, long_dx, lx, rx;
  V3D a, b, c;
  unsigned char *ptr, *ptrEnd;

  for (i = 0; i < 12; ++i)
  {
    color = color + i;

    a = verts[triList[i * 3 + 0]];
    b = verts[triList[i * 3 + 1]];
    c = verts[triList[i * 3 + 2]];

    // Shifting by 11 gets 65536 down to 64 which fits okay as a max 128 within 200 height
    a.x = (a.x >> 11) + (a.x >> 12) + (WIDTH >> 1);
    a.y = (a.y >> 11) + (a.y >> 12) + (HEIGHT >> 1);
    b.x = (b.x >> 11) + (b.x >> 12) + (WIDTH >> 1);
    b.y = (b.y >> 11) + (b.y >> 12) + (HEIGHT >> 1);
    c.x = (c.x >> 11) + (c.x >> 12) + (WIDTH >> 1);
    c.y = (c.y >> 11) + (c.y >> 12) + (HEIGHT >> 1);

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
    }
    if (a.y > c.y)
    {
      j = a.y;
      a.y = c.y;
      c.y = j;
      j = a.x;
      a.x = c.x;
      c.x = j;
    }
    if (b.y > c.y)
    {
      j = b.y;
      b.y = c.y;
      c.y = j;
      j = b.x;
      b.x = c.x;
      c.x = j;
    }

    int shortHeight = b.y - a.y;
    int longHeight = c.y - a.y;

    if (longHeight <= 0)
      continue;

    long_dx = (c.x - a.x) * oneover(longHeight);
    short_dx = (b.x - a.x) * oneover(shortHeight);
    lx = (a.x << 16);
    rx = (a.x << 16);
    ptr = &back[a.y * WIDTH];

    if (shortHeight > 0) // Top Half
    {
      if (long_dx < short_dx)
      { // Left side long edge
        do
        {
          j = ((rx - lx) >> 16);
          ptrEnd = ptr + (lx >> 16);
          while (j-- >= 0)
          {
            *ptrEnd++ = color + j;
          }

          lx += long_dx;
          rx += short_dx;
          ptr += WIDTH;
        } while (--shortHeight > 0);
      }
      else
      { // Right side long edge
        do
        {
          j = ((rx - lx) >> 16);
          ptrEnd = ptr + (lx >> 16);
          while (j-- >= 0)
          {
            *ptrEnd++ = color + j;
          }

          rx += long_dx;
          lx += short_dx;
          ptr += WIDTH;
        } while (--shortHeight > 0);
      }
    }

    // Bottom Half

    shortHeight = c.y - b.y;
    if (shortHeight > 0)
    {
      short_dx = (c.x - b.x) * oneover(shortHeight);

      if (short_dx < long_dx)
      { // Left side long edge
        rx = (b.x << 16);

        do
        {
          j = ((rx - lx) >> 16);
          ptrEnd = ptr + (lx >> 16);
          while (j-- >= 0)
          {
            *ptrEnd++ = color + j;
          }

          lx += long_dx;
          rx += short_dx;
          ptr += WIDTH;
        } while (--shortHeight > 0);
      }
      else
      { // Right side long edge
        lx = (b.x << 16);

        do
        {
          j = ((rx - lx) >> 16);
          ptrEnd = ptr + (lx >> 16);
          while (j-- >= 0)
          {
            *ptrEnd++ = color + j;
          }

          rx += long_dx;
          lx += short_dx;
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

  V3D cubeTransformed[8];
  MAT43 mat = {0};

  SetupTables();

  // for (i = 0; i < 8; ++i)
  // {
  //   cubeVerts[i].x = (cubeVerts[i].x << 5) + (cubeVerts[i].x << 4);
  //   cubeVerts[i].y = (cubeVerts[i].y << 5) + (cubeVerts[i].y << 4);
  //   cubeVerts[i].z = (cubeVerts[i].z << 5) + (cubeVerts[i].z << 4);
  // }

// Map physical 0xA0000 into our flat address space
#ifdef __DJGPP__
  __djgpp_nearptr_enable();
  vga = (unsigned char *)(__djgpp_conventional_base + 0xA0000);
#else // Watcom C/C++
  vga = (unsigned char *)0xA0000;
#endif

  // Set VGA mode 13h
  set_video_mode(0x13);

  // for (i = 0; i < 256; ++i)
  // {
  //   outportb(0x03C6, 0xFF);   // Write mask
  //   outportb(0x03C8, i);      // Color index
  //   outportb(0x03C9, 0);      // Red
  //   outportb(0x03C9, i >> 1); // Green
  //   outportb(0x03C9, i);      // Blue
  // }

  current_time = time(NULL);

  // Simple animation loop
  while (!kbhit())
  {
    EulerToMat(&mat, 256 + t, 32 + (t >> 1), 111 + (t >> 2));
    // EulerToMat(&mat, 0, 0, 0);

    memset(&back[0], 0, SIZE);

    for (i = 0; i < 8; ++i)
    {
      MultV3DMat(&cubeVerts[i], &cubeTransformed[i], &mat);
    }

    draw_tris(cubeTransformed, triList, 1);

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
