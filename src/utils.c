
#ifdef __DJGPP__
#include <dpmi.h>
#include <go32.h>
#include <pc.h>
#include <sys/farptr.h>
#else // Watcom C/C++
#include <i86.h>
#include <conio.h>
#endif

#include <stdio.h>

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif // min

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif // max

// Function to set the VGA video mode using BIOS interrupt 0x10
void SetVideoMode(int mode)
{
#ifdef __DJGPP__
  __dpmi_regs r;
  r.x.ax = mode;
  __dpmi_int(0x10, &r);
#else // Watcom C/C++
  union REGS regs;
  regs.w.ax = mode;           // AH=00h (Set Video Mode), AL=mode number
  int386(0x10, &regs, &regs); // Call BIOS interrupt 0x10 (Video Services)
#endif
}

void SetPalette(void)
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

int LoadPalette(const char *filename)
{
  FILE *file;
  char line[256];
  unsigned int index = 0;
  unsigned int r, g, b;

  file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("ERROR - Unable to find file.\n");
    return 1;
  }

  // Set the mask to allow all bits to be written
#ifdef __DJGPP__
  outportb(0x03C6, 0xFF); // Write mask
#else                     // Watcom C/C++
  outp(0x03C6, 0xFF); // Write mask
#endif

  // HEX file with 256 entries in the form RRGGBB
  while (fgets(line, 256, file))
  {
    if (sscanf(line, "%02X%02X%02X", &r, &g, &b) == 3)
    {
      r >>= 2;
      g >>= 2;
      b >>= 2;
      // VGA is 0-63 so we shift right by 2
#ifdef __DJGPP__
      outportb(0x03C8, index); // Color index
      outportb(0x03C9, r);     // Red
      outportb(0x03C9, g);     // Green
      outportb(0x03C9, b);     // Blue
#else                          // Watcom C/C++
      outp(0x03C8, index); // Color index
      outp(0x03C9, r);     // Red
      outp(0x03C9, g);     // Green
      outp(0x03C9, b);     // Blue
#endif
      printf("%d: %d %d %d\n", index, r, g, b);
      index++;
    }
  }

  fclose(file);

  return 0;
}

// Wait for vertical retrace to avoid tearing
void WaitVRetrace(void)
{
#ifdef __DJGPP__
  // Wait until not in vertical retrace
  while (inportb(0x3DA) & 0x08)
    ;
  // Wait until vertical retrace starts
  while (!(inportb(0x3DA) & 0x08))
    ;
#else // Watcom C/C++
  // Wait until not in vertical retrace
  while (inp(0x3DA) & 0x08)
    ;
  // Wait until vertical retrace starts
  while (!(inp(0x3DA) & 0x08))
    ;
#endif
}

// Returns the BIOS tick counter at 0040:006C (~18.2 ticks per second)
unsigned long GetTicks(void)
{
#ifdef __DJGPP__
  return _farpeekl(_dos_ds, 0x46C);
#else // Watcom C/C++
  return *(volatile unsigned long far *)0x0040006CL;
#endif
}
