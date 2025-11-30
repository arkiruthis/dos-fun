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
    {-65536, -65536, -65536},
    {65536, -65536, -65536},
    {65536, 65536, -65536},
    {-65536, 65536, -65536},
    {-65536, -65536, 65536},
    {65536, -65536, 65536},
    {65536, 65536, 65536},
    {-65536, 65536, 65536}};

// 12 edges of the cube (start and end vertex indices)
static int edgeList[24] = {
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    4, 5,
    5, 6,
    6, 7,
    7, 4,
    0, 4,
    1, 5,
    2, 6,
    3, 7};

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
    color += i;

    a = verts[triList[i * 3 + 0]];
    b = verts[triList[i * 3 + 1]];
    c = verts[triList[i * 3 + 2]];

    a.x += (WIDTH << 15);
    a.y += (HEIGHT << 15);
    b.x += (WIDTH << 15);
    b.y += (HEIGHT << 15);
    c.x += (WIDTH << 15);
    c.y += (HEIGHT << 15);

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

    int shortHeight = fix2int(b.y - a.y);
    while (shortHeight > 256)
    {
      shortHeight >>= 1;
    }
    int longheight = fix2int(c.y - a.y);
    while (longheight > 256)
    {
      longheight >>= 1;
    }

    long_dx = (c.x - a.x);
    short_dx = (b.x - a.x);
    long_dx = multOneOver(long_dx, longheight);
    short_dx = multOneOver(short_dx, shortHeight);

    if (long_dx < short_dx) // Left side long edge
    {
      lx = a.x;
      rx = a.x;
      ptr = &back[(a.y >> 16) * WIDTH];
      while (shortHeight-- > 0)
      {
        j = lx >> 16;
        k = rx >> 16;
        do
        {
          *(ptr + j) = color;
        } while ((++j) < k);

        ptr += WIDTH;
        lx += long_dx;
        rx += short_dx;
      };

      shortHeight = fix2int(c.y - b.y);
      while (shortHeight > 256)
      {
        shortHeight >>= 1;
      }

      short_dx = (c.x - b.x);
      short_dx = multOneOver(short_dx, shortHeight);
      rx = b.x;

      while (shortHeight-- > 0)
      {
        j = lx >> 16;
        k = rx >> 16;
        do
        {
          *(ptr + j) = color;
        } while ((++j) < k);

        ptr += WIDTH;
        lx += long_dx;
        rx += short_dx;
      };
    }
    else // Right side long edge
    {
      lx = a.x;
      rx = a.x;
      ptr = &back[(a.y >> 16) * WIDTH];
      while (shortHeight-- > 0)
      {
        j = lx >> 16;
        k = rx >> 16;
        do
        {
          *(ptr + j) = color;
        } while ((++j) < k);

        ptr += WIDTH;
        rx += long_dx;
        lx += short_dx;
      };

      shortHeight = fix2int(c.y - b.y);
      while (shortHeight > 256)
      {
        shortHeight >>= 1;
      }

      short_dx = (c.x - b.x);
      short_dx = multOneOver(short_dx, shortHeight);
      lx = b.x;

      while (shortHeight-- > 0)
      {
        j = lx >> 16;
        k = rx >> 16;
        do
        {
          *(ptr + j) = color;
        } while ((++j) < k);

        ptr += WIDTH;
        rx += long_dx;
        lx += short_dx;
      };
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

  for (i = 0; i < 8; ++i)
  {
    cubeVerts[i].x <<= 5;
    cubeVerts[i].y <<= 5;
    cubeVerts[i].z <<= 5;
  }

// Map physical 0xA0000 into our flat address space
#ifdef __DJGPP__
  __djgpp_nearptr_enable();
  vga = (unsigned char *)(__djgpp_conventional_base + 0xA0000);
#else // Watcom C/C++
  vga = (unsigned char *)0xA0000;
#endif

  // Set VGA mode 13h
  set_video_mode(0x13);

  for (i = 0; i < 256; ++i)
  {
    outportb(0x03C6, 0xFF);   // Write mask
    outportb(0x03C8, i);      // Color index
    outportb(0x03C9, 0);      // Red
    outportb(0x03C9, i >> 1); // Green
    outportb(0x03C9, i);      // Blue
  }

  current_time = time(NULL);

  // Simple animation loop
  while (!kbhit())
  {
    EulerToMat(&mat, 256 + (t >> 2), 32 + (t >> 3), 111 + (t >> 4));

    memset(&back[0], 0, SIZE);

    for (i = 0; i < 8; ++i)
    {
      MultV3DMat(&cubeVerts[i], &cubeTransformed[i], &mat);
    }

    draw_tris(cubeTransformed, triList, t);

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
