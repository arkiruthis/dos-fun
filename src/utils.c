
#ifdef __DJGPP__
#include <dpmi.h>
#include <go32.h>
#else // Watcom C/C++
#include <i86.h>
#endif

// Function to set the VGA video mode using BIOS interrupt 0x10
void set_video_mode(int mode)
{
#ifdef __DJGPP__
  __dpmi_regs r;
  r.x.ax = 0x0013;
  __dpmi_int(0x10, &r);
#else // Watcom C/C++
  union REGS regs;
  regs.w.ax = mode; // AH=00h (Set Video Mode), AL=mode number
  int386(0x10, &regs, &regs); // Call BIOS interrupt 0x10 (Video Services)
#endif
}
