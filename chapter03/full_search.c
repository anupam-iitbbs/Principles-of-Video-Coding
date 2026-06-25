/*
 * full_search.c
 * Exhaustive (full) block-matching motion search.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * Full search evaluates the matching criterion at EVERY integer
 * displacement in the window [-p, p] x [-p, p] and keeps the one with the
 * smallest SAD.  It is guaranteed optimal within the window, but its cost
 * grows as (2p+1)^2 candidate positions -- the motivation for the fast
 * searches studied later in the chapter.
 */

#include <stdio.h>
#include <limits.h>

#define W 64            /* reference frame width  */
#define H 64            /* reference frame height */
#define BS 16           /* block size (BS x BS)   */
#define P 16            /* search range +/- P     */

static int ref[H][W];   /* previously decoded reference frame */
static int cur[BS][BS]; /* current block to be matched        */

/* SAD between the current block and the reference block whose top-left
 * corner is at (ry, rx).  Returns INT_MAX if the block falls outside. */
static long sad_at(int ry, int rx)
{
    long sad = 0;
    int i, j;
    if (ry < 0 || rx < 0 || ry + BS > H || rx + BS > W)
        return LONG_MAX;
    for (i = 0; i < BS; i++)
        for (j = 0; j < BS; j++) {
            int d = cur[i][j] - ref[ry + i][rx + j];
            sad += (d < 0) ? -d : d;
        }
    return sad;
}

/* Full search around the block's own position (by0, bx0).
 * Writes the best motion vector into *bdy, *bdx and returns the best SAD. */
long full_search(int by0, int bx0, int *bdy, int *bdx)
{
    long best = LONG_MAX;
    int dy, dx;
    *bdy = 0; *bdx = 0;
    for (dy = -P; dy <= P; dy++)
        for (dx = -P; dx <= P; dx++) {
            long s = sad_at(by0 + dy, bx0 + dx);
            if (s < best) { best = s; *bdy = dy; *bdx = dx; }
        }
    return best;
}

int main(void)
{
    int i, j, bdy, bdx;
    long best;

    /* Synthetic test: a uniform reference, and a current block that matches
     * the reference content displaced by (dy, dx) = (3, -2). */
    for (i = 0; i < H; i++)
        for (j = 0; j < W; j++)
            ref[i][j] = (i * 7 + j * 3) & 0xFF;

    for (i = 0; i < BS; i++)
        for (j = 0; j < BS; j++)
            cur[i][j] = ref[24 + 3 + i][24 - 2 + j];  /* block at (24,24)+MV */

    best = full_search(24, 24, &bdy, &bdx);
    printf("best MV = (dx=%d, dy=%d), SAD = %ld\n", bdx, bdy, best);
    return 0;
}
