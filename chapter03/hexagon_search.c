/*
 * hexagon_search.c
 * Hexagon-Based Search (HEXBS), the default integer-pel search in x264.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * A hexagon of 6 surrounding points covers the directions around the
 * centre more evenly than a diamond.  The large hexagon is applied with
 * recentring until its best point is the centre; a small 4-point pattern
 * then refines to pixel accuracy.  Because a recentred hexagon shares
 * three points with the previous step, much of the work is reused.
 */

#include <stdio.h>
#include <limits.h>

#define RW 64
#define RH 64
#define BS 16

static int ref[RH][RW];
static int cur[BS][BS];

static long sad_at(int ry, int rx)
{
    long sad = 0; int i, j;
    if (ry < 0 || rx < 0 || ry + BS > RH || rx + BS > RW) return LONG_MAX;
    for (i = 0; i < BS; i++)
        for (j = 0; j < BS; j++) {
            int d = cur[i][j] - ref[ry + i][rx + j];
            sad += (d < 0) ? -d : d;
        }
    return sad;
}

/* Large hexagon (6 points) and small refinement pattern (4 points). */
static const int hex[6][2] = {
    {2,0},{1,2},{-1,2},{-2,0},{-1,-2},{1,-2}
};
static const int small4[4][2] = { {0,1},{1,0},{0,-1},{-1,0} };

long hexagon_search(int by0, int bx0, int *bdy, int *bdx)
{
    int cy = 0, cx = 0, k;
    long best = sad_at(by0, bx0);
    for (;;) {                        /* repeated large hexagon */
        int by = cy, bx = cx; long lb = best;
        for (k = 0; k < 6; k++) {
            int ty = cy + hex[k][1], tx = cx + hex[k][0];
            long s = sad_at(by0 + ty, bx0 + tx);
            if (s < lb) { lb = s; by = ty; bx = tx; }
        }
        if (by == cy && bx == cx) break;
        best = lb; cy = by; cx = bx;
    }
    {                                 /* small final refinement */
        int by = cy, bx = cx; long lb = best;
        for (k = 0; k < 4; k++) {
            int ty = cy + small4[k][1], tx = cx + small4[k][0];
            long s = sad_at(by0 + ty, bx0 + tx);
            if (s < lb) { lb = s; by = ty; bx = tx; }
        }
        best = lb; cy = by; cx = bx;
    }
    *bdy = cy; *bdx = cx;
    return best;
}

int main(void)
{
    int i, j, bdy, bdx; long best;
    for (i = 0; i < RH; i++)
        for (j = 0; j < RW; j++)
            ref[i][j] = (i * 7 + j * 3) & 0xFF;
    for (i = 0; i < BS; i++)
        for (j = 0; j < BS; j++)
            cur[i][j] = ref[24 + 7 + i][24 + 5 + j];  /* MV = (dx=5, dy=7) */
    best = hexagon_search(24, 24, &bdy, &bdx);
    printf("HEXBS MV = (dx=%d, dy=%d), SAD = %ld\n", bdx, bdy, best);
    return 0;
}
