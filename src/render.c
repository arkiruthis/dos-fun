#include "render.h"
#include "math3d.h"

#define EDGELIST_SIZE 200 // 200 scanlines max
EDGE g_edgeList[EDGELIST_SIZE];

unsigned char back[BACKBUFFER_SIZE];

void hline(int length, fix c1, fix c2, unsigned char *ptr)
{
    fix xstep = (((c2 - c1) >> 8) + 1) * oneover(length);

    do
    {
        *ptr++ = (c1 >> 8);
        c1 += xstep;
    } while (length-- > 0);
}

void run_edge_list(fix yStart, fix height)
{
    EDGE *currentEdge = &g_edgeList[yStart];

    do
    {
        fix span = currentEdge->span;
        fix c1 = currentEdge->c;
        fix xstep = currentEdge->xStep;
        unsigned char *linePtr = currentEdge->ptr;

        do
        {
            *linePtr++ = (c1 >> 8);
            c1 += xstep;
        } while (span-- > 0);
        ++currentEdge;
    } while (--height > 0);
}

void draw_tris(V4D *verts, int triList[])
{
    fix i, j, k;
    fix short_dx, long_dx, lx, rx;
    fix long_cx, short_cx, lc, rc;
    fix shortHeight, longHeight;
    V4D a, b, c;
    unsigned char *ptr, *ptrEnd;
    EDGE *currentEdge;

    for (i = 0; i < 12; ++i)
    {
        a = verts[triList[(i * 3) + 0]];
        b = verts[triList[(i * 3) + 1]];
        c = verts[triList[(i * 3) + 2]];

        // Shifting by 11 gets 65536 down to 64 which fits okay as a max 128 within 200 height
        a.x = (a.x >> 2) + (BACKBUFFER_WIDTH >> 1);
        a.y = (a.y >> 2) + (BACKBUFFER_HEIGHT >> 1);
        a.z = min(max(24 - (a.z >> 3), 0), 63);
        b.x = (b.x >> 2) + (BACKBUFFER_WIDTH >> 1);
        b.y = (b.y >> 2) + (BACKBUFFER_HEIGHT >> 1);
        b.z = min(max(24 - (b.z >> 3), 0), 63);
        c.x = (c.x >> 2) + (BACKBUFFER_WIDTH >> 1);
        c.y = (c.y >> 2) + (BACKBUFFER_HEIGHT >> 1);
        c.z = min(max(24 - (c.z >> 3), 0), 63);

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
        ptr = &back[a.y * BACKBUFFER_WIDTH];

        if (shortHeight > 0) // Top Half
        {
            short_dx = (b.x - a.x) * oneover16(shortHeight);
            short_cx = (b.z - a.z) * oneover(shortHeight);

            if (long_dx < short_dx)
            { // Left side long edge
                do
                {
                    j = ((rx - lx) >> 16);
                    ptrEnd = ptr + (lx >> 16);
                    hline(j, lc, rc, ptrEnd);

                    lx += long_dx;
                    rx += short_dx;
                    lc += long_cx;
                    rc += short_cx;
                    ptr += BACKBUFFER_WIDTH;
                } while (--shortHeight > 0);
            }
            else
            { // Right side long edge
                do
                {
                    j = ((rx - lx) >> 16);
                    ptrEnd = ptr + (lx >> 16);
                    hline(j, lc, rc, ptrEnd);

                    rx += long_dx;
                    lx += short_dx;
                    rc += long_cx;
                    lc += short_cx;
                    ptr += BACKBUFFER_WIDTH;
                } while (--shortHeight > 0);
            }
        }

        // Bottom Half

        shortHeight = c.y - b.y;
        if (shortHeight > 0)
        {
            short_dx = (c.x - b.x) * oneover16(shortHeight);
            short_cx = (c.z - b.z) * oneover(shortHeight);

            if (short_dx < long_dx)
            { // Left side long edge
                rx = (b.x << 16);
                rc = (b.z << 8);

                do
                {
                    j = ((rx - lx) >> 16);
                    ptrEnd = ptr + (lx >> 16);
                    hline(j, lc, rc, ptrEnd);

                    lx += long_dx;
                    rx += short_dx;
                    lc += long_cx;
                    rc += short_cx;
                    ptr += BACKBUFFER_WIDTH;
                } while (--shortHeight > 0);
            }
            else
            { // Right side long edge
                lx = (b.x << 16);
                lc = (b.z << 8);

                do
                {
                    j = ((rx - lx) >> 16);
                    ptrEnd = ptr + (lx >> 16);
                    hline(j, lc, rc, ptrEnd);

                    rx += long_dx;
                    lx += short_dx;
                    rc += long_cx;
                    lc += short_cx;
                    ptr += BACKBUFFER_WIDTH;
                } while (--shortHeight > 0);
            }
        }
    }
}