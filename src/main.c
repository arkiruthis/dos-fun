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
#include "render.h"

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

static V4D cubeVertNormals[8] = {
    { -147, -147, 147, 0},   // FRONT TOP LEFT
    { 147, -147, 147, 0},    // FRONT TOP RIGHT
    { 147, 147, 147, 0},     // FRONT BOTTOM RIGHT
    { -147, 147, 147, 0},    // FRONT BOTTOM LEFT
    { -147, -147, -147, 0},  // BACK TOP LEFT
    { 147, -147, -147, 0},   // BACK TOP RIGHT
    { 147, 147, -147, 0},    // BACK BOTTOM RIGHT
    { -147, 147, -147, 0},   // BACK BOTTOM LEFT
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

int main(void)
{
  unsigned char *vga;
  unsigned char *ptr;
  int i, x, y;
  int t = 0;
  time_t current_time;

  V4D cubeTransformed[8];
  V4D normalsTransformed[8];
  V3D lightDir = {float2fix(0.0f), float2fix(0.0f), float2fix(-1.0f)};
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
  SetVideoMode(0x13);

  SetPalette();

  current_time = time(NULL);

  // Simple animation loop
  while (!kbhit())
  {
    EulerToMat(&mat, 256 + t, 55 + t, 77 + (t >> 2));
    // EulerToMat(&mat, 0, 0, 0);

    memset(&back[0], 0, BACKBUFFER_SIZE);

    for (i = 0; i < 8; ++i)
    {
      MultV4DMatC(&cubeVerts[i], &cubeTransformed[i], &mat);
      MultV4DMatC(&cubeVertNormals[i], &normalsTransformed[i], &mat);
      cubeTransformed[i].z = max(0, DotProduct((V3D *)&normalsTransformed[i], &lightDir));
    }

    DrawTris(cubeTransformed, triList);

    // Wait for vertical retrace to avoid tearing
    WaitVRetrace();

    // Blit back buffer -> VGA in one go
    memcpy(vga, &back[0], BACKBUFFER_SIZE);
    ++t;
  }
  current_time = time(NULL) - current_time;

  getch();

  // Back to text mode 3
  SetVideoMode(0x03);

  printf("Returned to text mode. Program finished.\n");
  printf("Elapsed time: %ld seconds\n", current_time);
  printf("Frame rate: %.2f FPS\n", (double)t / (double)current_time);

  return 0;
}
