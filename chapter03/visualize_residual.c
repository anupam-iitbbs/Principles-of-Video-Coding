/*
 * visualize_residual.c
 * Compute the frame difference / residual and write it as a viewable
 * grayscale PGM image.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * Two modes of difference are shown:
 *   1. RAW frame difference   d = x_n - x_(n-1)   (no motion compensation)
 *   2. RESIDUAL after MC       e = x_n - xhat       (with a motion vector)
 * Both are shifted by +128 so that zero maps to mid-gray and the result
 * can be saved as an 8-bit PGM and inspected.  The motion-compensated
 * residual should be visibly flatter than the raw difference.
 */

#include <stdio.h>

#define W 16
#define H 16

/* Clamp v into [0, 255]. */
static int clamp8(int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); }

/* Write a W x H grayscale buffer as a binary PGM file. */
static void write_pgm(const char *path, const int *img, int w, int h)
{
    FILE *f = fopen(path, "wb");
    int i;
    if (!f) { perror("fopen"); return; }
    fprintf(f, "P5\n%d %d\n255\n", w, h);
    for (i = 0; i < w * h; i++)
        fputc(clamp8(img[i]), f);
    fclose(f);
}

int main(void)
{
    int prev[H * W], curr[H * W], diff[H * W], resid[H * W];
    int i, j, dy = 1, dx = 1;        /* assumed motion vector */

    /* Synthetic frames: a smooth ramp that shifts by (dy, dx). */
    for (i = 0; i < H; i++)
        for (j = 0; j < W; j++) {
            prev[i * W + j] = (i * 8 + j * 8) & 0xFF;
            curr[i * W + j] = ((i - dy) * 8 + (j - dx) * 8) & 0xFF;
        }

    /* Raw difference and motion-compensated residual. */
    for (i = 0; i < H; i++)
        for (j = 0; j < W; j++) {
            int p = prev[i * W + j];
            int ry = i + dy, rx = j + dx;
            int mc = (ry < H && rx < W) ? prev[ry * W + rx] : p;
            diff[i * W + j]  = curr[i * W + j] - p  + 128;
            resid[i * W + j] = curr[i * W + j] - mc + 128;
        }

    write_pgm("raw_difference.pgm", diff, W, H);
    write_pgm("mc_residual.pgm",   resid, W, H);
    printf("Wrote raw_difference.pgm and mc_residual.pgm\n");
    return 0;
}
