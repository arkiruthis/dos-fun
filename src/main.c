#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef __DJGPP__
#include <sys/nearptr.h>
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

void draw_lines(V3D *verts, int edgeList[], unsigned char color)
{
  int i, j, height;
  fix x0, y0, x1, y1;
  fix dx, dy;
  unsigned char *ptr;

  for (i = 0; i < 12; ++i)
  {
    x0 = verts[edgeList[i * 2]].x + int2fix(WIDTH / 2);
    y0 = verts[edgeList[i * 2]].y + int2fix(HEIGHT / 2);
    x1 = verts[edgeList[i * 2 + 1]].x + int2fix(WIDTH / 2);
    y1 = verts[edgeList[i * 2 + 1]].y + int2fix(HEIGHT / 2);

    if (y0 > y1)
    {
      // Swap points so y0 is always <= y1
      dx = x0;
      dy = y0;
      x0 = x1;
      y0 = y1;
      x1 = dx;
      y1 = dy;
    }

    dx = x1 - x0;
    height = (y1 - y0) >> 16;
    if (height == 0) // flat line
    {
      x0 >>= 16;
      x1 >>= 16;
      ptr = &back[(y0 >> 16) * WIDTH];
      ptr += (x0 < x1) ? x0 : x1;
      height = abs(dx >> 16);
      do {
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
}

void draw_verts(V3D *verts, int numVerts, unsigned char color)
{
  int i;
  for (i = 0; i < numVerts; ++i)
  {
    int x = (verts[i].x >> 11) + WIDTH / 2;
    int y = (verts[i].y >> 11) + HEIGHT / 2;
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
    {
      back[y * WIDTH + x] = color;
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

  current_time = time(NULL);

  // Simple animation loop
  while (!kbhit())
  {
    EulerToMat(&mat, 256 + t, 32 + t, 111 + t);

    memset(&back[0], 0, SIZE);

    for (i = 0; i < 8; ++i)
    {
      MultV3DMat(&cubeVerts[i], &cubeTransformed[i], &mat);
    }

    draw_lines(cubeTransformed, edgeList, 15);

    // Blit back buffer -> VGA in one go
    memcpy(vga, &back[0], SIZE);
    ++t;
  }
  current_time = time(NULL) - current_time;

  printf("Returned to text mode. Program finished.\n");
  printf("Elapsed time: %ld seconds\n", current_time);
  printf("Frame rate: %.2f FPS\n", 1000.0 / (double)current_time);

  getch();

  // Back to text mode 3
  set_video_mode(0x03);

  return 0;
}
