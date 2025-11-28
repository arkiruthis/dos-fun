#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __DJGPP__
#include <sys/nearptr.h>
#endif

#include "utils.h"

#define WIDTH 320
#define HEIGHT 200
#define SIZE (WIDTH * HEIGHT)

static unsigned char back[SIZE];

int main(void)
{
  unsigned char *vga;
  unsigned char *ptr;
  int i, x, y;
  int t = 0;
  time_t current_time;

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
  for (t = 0; t < 1000; ++t)
  {
    ptr = &back[0];
    // Draw into back buffer only
    for (i = 0; i < SIZE; ++i)
    {
      x = i;
      y = i / WIDTH;
      *ptr++ = (unsigned char)((x ^ t) + y);
    }

    // Blit back buffer -> VGA in one go
    memcpy(vga, &back[0], SIZE);
  }
  current_time = time(NULL) - current_time;

  printf("Returned to text mode. Program finished.\n");
  printf("Elapsed time: %ld seconds\n", current_time);

  getch();

  // Back to text mode 3
  set_video_mode(0x03);

  return 0;
}
