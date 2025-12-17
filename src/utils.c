
#ifdef __DJGPP__
#include <dpmi.h>
#include <go32.h>
#else // Watcom C/C++
#include <i86.h>
#include <conio.h>
#endif

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif // min

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif // max

// Function to set the VGA video mode using BIOS interrupt 0x10
void set_video_mode(int mode)
{
#ifdef __DJGPP__
  __dpmi_regs r;
  r.x.ax = mode;
  __dpmi_int(0x10, &r);
#else // Watcom C/C++
  union REGS regs;
  regs.w.ax = mode; // AH=00h (Set Video Mode), AL=mode number
  int386(0x10, &regs, &regs); // Call BIOS interrupt 0x10 (Video Services)
#endif
}

void set_palette(void)
{
  int i;

#ifdef __DJGPP__
  outportb(0x03C6, 0xFF); // Write mask
  for (i = 0; i < 64; ++i)
  {
    outportb(0x03C8, i);               // Color index
    outportb(0x03C9, min(i >> 1, 63)); // Red
    outportb(0x03C9, min(i, 63));      // Green
    outportb(0x03C9, min(i >> 2, 63)); // Blue
  }
#else // Watcom C/C++
  outp(0x03C6, 0xFF); // Write mask
  for (i = 0; i < 64; ++i)
  {
    outp(0x03C8, i);               // Color index
    outp(0x03C9, min(i >> 1, 63)); // Red
    outp(0x03C9, min(i, 63));      // Green
    outp(0x03C9, min(i >> 2, 63)); // Blue
  }
#endif
}
