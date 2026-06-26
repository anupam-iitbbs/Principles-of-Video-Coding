/*
 * sad_surface.c
 * Visualize the SAD surface over the search window.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * For every candidate displacement (dx, dy) in the search window we compute
 * the SAD between the current block and the reference block at that offset,
 * then write the whole surface as a grayscale PGM image.  Low SAD (good
 * match) appears dark; high SAD (poor match) appears bright.  The result is
 * the distortion landscape that fast searches descend -- usually a single
 * smooth basin around the true minimum.
 */

#include <stdio.h>
#include <limits.h>

#define RW 64
#define RH 64
#define BS 16
#define P 16                 /* search range +/- P */
#define N (2 * P + 1)        /* surface side length */

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

int main(void)
{
    long surf[N][N];
    long lo = LONG_MAX, hi = 0;
    int i, j, dy, dx;
    FILE *f;

    for (i = 0; i < RH; i++)
        for (j = 0; j < RW; j++)
            ref[i][j] = (i * 7 + j * 3) & 0xFF;
    for (i = 0; i < BS; i++)
        for (j = 0; j < BS; j++)
            cur[i][j] = ref[24 + 4 + i][24 - 3 + j];  /* true MV (dx=-3, dy=4) */

    /* Build the SAD surface and track its finite range for scaling. */
    for (dy = -P; dy <= P; dy++)
        for (dx = -P; dx <= P; dx++) {
            long s = sad_at(24 + dy, 24 + dx);
            surf[dy + P][dx + P] = s;
            if (s != LONG_MAX) { if (s < lo) lo = s; if (s > hi) hi = s; }
        }

    /* Write as PGM: dark = low SAD (good), bright = high SAD (poor). */
    f = fopen("sad_surface.pgm", "wb");
    if (!f) { perror("fopen"); return 1; }
    fprintf(f, "P5\n%d %d\n255\n", N, N);
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            long s = surf[i][j];
            int v = (s == LONG_MAX || hi == lo) ? 255
                    : (int)(255L * (s - lo) / (hi - lo));
            fputc(v, f);
        }
    fclose(f);
    printf("Wrote sad_surface.pgm (min SAD = %ld, max SAD = %ld)\n", lo, hi);
    return 0;
}
