/*
 * three_step.c
 * Three-Step Search (TSS) for block-based motion estimation.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * The classic fast search: start with a large step, evaluate the centre
 * and eight surrounding positions, jump to the best, halve the step, and
 * repeat.  Three rounds reduce the step from 4 to 1.  TSS tests about 25
 * positions instead of the thousands a full search needs, at the risk of
 * settling into a local minimum.
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

/* Three-step search around (by0, bx0).  Returns best SAD; writes MV. */
long three_step_search(int by0, int bx0, int *bdy, int *bdx)
{
    int cy = 0, cx = 0;        /* current best displacement */
    int step = 4;              /* initial step size         */
    long best = sad_at(by0, bx0);
    static const int off[8][2] = {
        {-1,-1},{0,-1},{1,-1},{-1,0},{1,0},{-1,1},{0,1},{1,1}
    };
    while (step >= 1) {
        int by = cy, bx = cx, k;
        long localbest = best;
        for (k = 0; k < 8; k++) {
            int ty = cy + off[k][1] * step;
            int tx = cx + off[k][0] * step;
            long s = sad_at(by0 + ty, bx0 + tx);
            if (s < localbest) { localbest = s; by = ty; bx = tx; }
        }
        best = localbest; cy = by; cx = bx;
        step /= 2;
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
            cur[i][j] = ref[24 + 3 + i][24 + 4 + j];  /* MV = (dx=4, dy=3) */
    best = three_step_search(24, 24, &bdy, &bdx);
    printf("TSS MV = (dx=%d, dy=%d), SAD = %ld\n", bdx, bdy, best);
    return 0;
}
