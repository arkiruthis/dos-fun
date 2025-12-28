#ifndef UTILS_H
#define UTILS_H

// BIOS tick rate: 1193182 / 65536 ≈ 18.2065 ticks per second
#define TICKS_PER_SEC 18.2065

void SetVideoMode(int mode);

int LoadPalette(const char* filename);

void SetPalette(void);

void WaitVRetrace(void);

// Returns the BIOS tick counter (~18.2 ticks per second)
unsigned long GetTicks(void);

#endif // UTILS_H
