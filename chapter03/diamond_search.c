/*
 * diamond_search.c
 * Diamond Search (DS) for block-based motion estimation.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * Uses a Large Diamond Pattern (LDP, 8 points at L1-distance 2) applied
 * repeatedly with recentring until the best point is the centre, then a
 * single Small Diamond Pattern (SDP, 4 points at distance 1) refinement.
 * Adaptive: low-motion blocks converge in one step, high-motion blocks
 * walk as far as they need.
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

/* Large diamond: centre plus 8 points; small diamond: 4 points. */
static const int ldp[8][2] = {
    {0,2},{2,0},{0,-2},{-2,0},{1,1},{1,-1},{-1,1},{-1,-1}
};
static const int sdp[4][2] = { {0,1},{1,0},{0,-1},{-1,0} };

long diamond_search(int by0, int bx0, int *bdy, int *bdx)
{
    int cy = 0, cx = 0, k;
    long best = sad_at(by0, bx0);
    for (;;) {                       /* repeated large diamond */
        int by = cy, bx = cx; long lb = best;
        for (k = 0; k < 8; k++) {
            int ty = cy + ldp[k][1], tx = cx + ldp[k][0];
            long s = sad_at(by0 + ty, bx0 + tx);
            if (s < lb) { lb = s; by = ty; bx = tx; }
        }
        if (by == cy && bx == cx) break;  /* centre is best: stop */
        best = lb; cy = by; cx = bx;
    }
    {                                 /* final small diamond refinement */
        int by = cy, bx = cx; long lb = best;
        for (k = 0; k < 4; k++) {
            int ty = cy + sdp[k][1], tx = cx + sdp[k][0];
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
            cur[i][j] = ref[24 - 5 + i][24 + 6 + j];  /* MV = (dx=6, dy=-5) */
    best = diamond_search(24, 24, &bdy, &bdx);
    printf("DS MV = (dx=%d, dy=%d), SAD = %ld\n", bdx, bdy, best);
    return 0;
}
