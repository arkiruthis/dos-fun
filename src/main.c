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
#include "mesh.h"

int main(void)
{
  unsigned char *vga;
  unsigned char *ptr;
  int i, x, y;
  int t = 0;
  time_t current_time;

  V4D cubeTransformed[8];
  V4D normalsTransformed[8];
  V3D lightDir = {float2fix(0.707f), float2fix(0.0f), -float2fix(0.707f)};
  MAT43 mat = {0};

  SetupTables();

  LoadObj("sphere.obj");

// Map physical 0xA0000 into our flat address space
#ifdef __DJGPP__
  __djgpp_nearptr_enable();
  vga = (unsigned char *)(__djgpp_conventional_base + 0xA0000);
#else // Watcom C/C++
  vga = (unsigned char *)0xA0000;
#endif

  // Set VGA mode 13h
  SetVideoMode(0x13);

  // SetPalette();
  if (LoadPalette("pal.hex") != 0)
  {
    printf("Failed to load palette file.\n");
    return 1;
  }

  current_time = time(NULL);

  // Simple animation loop
  while (!kbhit())
  {
    EulerToMat(&mat, 256 + t, 55 + t, 77 + (t >> 2));
    // EulerToMat(&mat, 0, 0, 0);

    memset(&back[0], 0, BACKBUFFER_SIZE);

    for (i = 0; i < cvector_size(g_Mesh.verts); ++i)
    {
      MultV4DMatC(&g_Mesh.verts[i], &g_Mesh.vertsTransformed[i], &mat);
      MultV4DMatC(&g_Mesh.vertNormals[i], &g_Mesh.vertNormalsTransformed[i], &mat);
      g_Mesh.vertsTransformed[i].z = max(0, DotProduct((V3D *)&g_Mesh.vertNormalsTransformed[i], &lightDir));
    }

    DrawTris(g_Mesh.vertsTransformed, g_Mesh.faces, cvector_size(g_Mesh.faces));

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

  FreeMesh();

  printf("Returned to text mode. Program finished.\n");
  printf("Elapsed time: %ld seconds\n", current_time);
  printf("Frame rate: %.2f FPS\n", (double)t / (double)current_time);

  return 0;
}
