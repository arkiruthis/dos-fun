#include "render.h"
#include "math3d.h"
#include "mesh.h"

unsigned char back[BACKBUFFER_SIZE];
TRI *triList[TRI_LIST_SIZE];

static inline void hline(int length, fix c1, fix c2, unsigned char *ptr)
{
    fix xstep = ((c2 - c1) * oneover(length)) >> 8;

    while (length-- > 0)
    {
        *ptr++ = (c1 >> 8);
        c1 += xstep;
    }
}

void DrawTris()
{
    fix i, j, k;
    fix short_dx, long_dx, lx, rx;
    fix long_cx, short_cx, lc, rc;
    fix shortHeight, longHeight;
    V4D a, b, c;
    unsigned char *ptr, *ptrEnd;

    //memset(&triList[0], 0, TRI_LIST_SIZE * sizeof(TRI*));

    for (i = 0; i < cvector_size(g_Mesh.faces); ++i)
    {
        a = g_Mesh.vertsTransformed[g_Mesh.faces[i].a];
        b = g_Mesh.vertsTransformed[g_Mesh.faces[i].b];
        c = g_Mesh.vertsTransformed[g_Mesh.faces[i].c];
        j = g_Mesh.faces[i].material_offset;

        // Shifting by 11 gets 65536 down to 64 which fits okay as a max 128 within 200 height
        a.x = (a.x >> 2);
        a.y = (a.y >> 2);
        b.x = (b.x >> 2);
        b.y = (b.y >> 2);
        c.x = (c.x >> 2);
        c.y = (c.y >> 2);

        a.w += j;
        b.w += j;
        c.w += j;

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
            j = a.w;
            a.w = b.w;
            b.w = j;
        }
        if (a.y > c.y)
        {
            j = a.y;
            a.y = c.y;
            c.y = j;
            j = a.x;
            a.x = c.x;
            c.x = j;
            j = a.w;
            a.w = c.w;
            c.w = j;
        }
        if (b.y > c.y)
        {
            j = b.y;
            b.y = c.y;
            c.y = j;
            j = b.x;
            b.x = c.x;
            c.x = j;
            j = b.w;
            b.w = c.w;
            c.w = j;
        }

        shortHeight = b.y - a.y;
        longHeight = c.y - a.y;

        if (longHeight <= 0)
            continue;

        long_dx = (c.x - a.x) * oneover16(longHeight);
        lx = (a.x << 16);
        rx = (a.x << 16);
        long_cx = (c.w - a.w) * oneover(longHeight);
        lc = (a.w << 8);
        rc = (a.w << 8);
        ptr = back;

        ptr += (BACKBUFFER_SIZE + BACKBUFFER_WIDTH) >> 1; // Center horizontally
        ptr += (a.y * BACKBUFFER_WIDTH);

        if (shortHeight > 0) // Top Half
        {
            short_dx = (b.x - a.x) * oneover16(shortHeight);
            short_cx = (b.w - a.w) * oneover(shortHeight);

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
            short_cx = (c.w - b.w) * oneover(shortHeight);

            rx = (b.x << 16);
            rc = (b.w << 8);

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

// void SubmitTriangle(V4D *verts, TRI *triList)
// {
//     triList->a = verts[0].x;
//     triList->b = verts[1].x;
//     triList->c = verts[2].x;
//     triList->next = NULL;
// }