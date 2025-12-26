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
  int i;
  int t = 0;
  time_t current_time;
  V3D lightDir = {float2fix(0.707f), float2fix(0.0f), -float2fix(0.707f)};
  MAT43 mat = {0};

  SetupTables();

  LoadObj("sphere.obj");

// Map physical 0xA0000 into our flat address space
#ifdef __DJGPP__
  __djgpp_nearptr_enable();
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
    // EulerToMat(&mat, 256 + t, 55 + t, 77 + (t >> 2));
    EulerToMat(&mat, t, 0, 20);

    for (i = 0; i < cvector_size(g_Mesh.verts); ++i)
    {
      MultV4DMatC(&g_Mesh.verts[i], &g_Mesh.vertsTransformed[i], &mat);
      MultV4DMatC(&g_Mesh.vertNormals[i], &g_Mesh.vertNormalsTransformed[i], &mat);
      g_Mesh.vertsTransformed[i].w = max(0, DotProduct((V3D *)&g_Mesh.vertNormalsTransformed[i], &lightDir));
      g_Mesh.vertsTransformed[i].w = min(g_Mesh.vertsTransformed[i].w >> 5, 15);
    }

    DrawTris();

    // Wait for vertical retrace to avoid tearing
    WaitVRetrace();

    // Blit backBuffer buffer -> VGA in one go
    BlitBackBufferToVGA();
    ++t;
  }
  current_time = time(NULL) - current_time;

  getch();

  // Back to text mode 3
  SetVideoMode(0x03);

  FreeMesh();

  printf("Returned to text mode. Program finished.\n");
  printf("Elapsed time: %u seconds\n", current_time);
  printf("Frame rate: %.2f FPS\n", (double)t / (double)current_time);

  return 0;
}
