#include "render.h"
#include "math3d.h"

unsigned char back[BACKBUFFER_SIZE];

inline void hline(int length, fix c1, fix c2, unsigned char *ptr)
{
    fix xstep = ((c2 - c1) * oneover(length)) >> 8;

    while (length-- > 0)
    {
        *ptr++ = (c1 >> 8);
        c1 += xstep;
    }
}

void DrawTris(V4D *verts, TRI *triList, size_t triCount)
{
    fix i, j, k;
    fix short_dx, long_dx, lx, rx;
    fix long_cx, short_cx, lc, rc;
    fix shortHeight, longHeight;
    V4D a, b, c;
    unsigned char *ptr, *ptrEnd;

    for (i = 0; i < triCount; ++i)
    {
        a = verts[triList[i].a];
        b = verts[triList[i].b];
        c = verts[triList[i].c];

        // Shifting by 11 gets 65536 down to 64 which fits okay as a max 128 within 200 height
        a.x = (a.x >> 2);
        a.y = (a.y >> 2);
        a.z = min(max(16 + (a.z >> 4), 0), 31);
        b.x = (b.x >> 2);
        b.y = (b.y >> 2);
        b.z = min(max(16 + (b.z >> 4), 0), 31);
        c.x = (c.x >> 2);
        c.y = (c.y >> 2);
        c.z = min(max(16 + (c.z >> 4), 0), 31);

        if (orient2dint(a, b, c) > 0)
            continue;

        // Sort vertices by Y
        if (a.y > b.y)
        {
            j = a.y;
            a.y = b.y;
            b.y = j;
            j = a.x;
            a.x = b.x;
            b.x = j;
            j = a.z;
            a.z = b.z;
            b.z = j;
        }
        if (a.y > c.y)
        {
            j = a.y;
            a.y = c.y;
            c.y = j;
            j = a.x;
            a.x = c.x;
            c.x = j;
            j = a.z;
            a.z = c.z;
            c.z = j;
        }
        if (b.y > c.y)
        {
            j = b.y;
            b.y = c.y;
            c.y = j;
            j = b.x;
            b.x = c.x;
            c.x = j;
            j = b.z;
            b.z = c.z;
            c.z = j;
        }

        shortHeight = b.y - a.y;
        longHeight = c.y - a.y;

        if (longHeight <= 0)
            continue;

        long_dx = (c.x - a.x) * oneover16(longHeight);
        lx = (a.x << 16);
        rx = (a.x << 16);
        long_cx = (c.z - a.z) * oneover(longHeight);
        lc = (a.z << 8);
        rc = (a.z << 8);
        ptr = back;

        ptr += (BACKBUFFER_SIZE + BACKBUFFER_WIDTH) >> 1; // Center horizontally
        ptr += (a.y * BACKBUFFER_WIDTH);


        if (shortHeight > 0) // Top Half
        {
            short_dx = (b.x - a.x) * oneover16(shortHeight);
            short_cx = (b.z - a.z) * oneover(shortHeight);

            do
            {
                j = abs((rx >> 16) - (lx >> 16));
                k = min(lx, rx);
                ptrEnd = ptr + (k >> 16);
                if (lx <= rx)
                    hline(j, lc, rc, ptrEnd);
                else
                    hline(j, rc, lc, ptrEnd);

                lx += long_dx;
                rx += short_dx;
                lc += long_cx;
                rc += short_cx;
                ptr += BACKBUFFER_WIDTH;
            } while (--shortHeight > 0);
        }

        // Bottom Half

        shortHeight = c.y - b.y;
        if (shortHeight > 0)
        {
            short_dx = (c.x - b.x) * oneover16(shortHeight);
            short_cx = (c.z - b.z) * oneover(shortHeight);

            rx = (b.x << 16);
            rc = (b.z << 8);

            do
            {
                j = abs((rx >> 16) - (lx >> 16));
                k = min(lx, rx);
                ptrEnd = ptr + (k >> 16);
                if (lx <= rx)
                    hline(j, lc, rc, ptrEnd);
                else
                    hline(j, rc, lc, ptrEnd);

                lx += long_dx;
                rx += short_dx;
                lc += long_cx;
                rc += short_cx;
                ptr += BACKBUFFER_WIDTH;
            } while (--shortHeight > 0);
        }
    }
}