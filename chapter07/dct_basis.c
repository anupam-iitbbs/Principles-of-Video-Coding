/*
 * dct_basis.c -- Generate the 64 two-dimensional 8x8 DCT basis images and
 * write them as a single PGM montage. Companion code for Chapter 7,
 * "Transform Coding".
 *
 * Each basis image B_{u,v}(m,n) = cos[pi(2m+1)u/16] * cos[pi(2n+1)v/16].
 * Smoothness increases towards the top-left (u=v=0, the DC basis);
 * frequency increases towards the bottom-right.
 *
 * Output: dct_basis.pgm -- an 8x8 grid of 8x8 tiles (64x64 pixels), with
 * 1-pixel separators, viewable in any image viewer.
 */
#include <stdio.h>
#include <math.h>

#define N 8
#define TILE (N + 1)          /* 8 px tile + 1 px separator */
#define DIM  (N * TILE + 1)   /* full montage side length */

static double basis(int u, int v, int m, int n)
{
    return cos(M_PI * (2 * m + 1) * u / (2.0 * N)) *
           cos(M_PI * (2 * n + 1) * v / (2.0 * N));
}

int main(void)
{
    static unsigned char img[DIM][DIM];

    /* white background acts as the grid lines */
    for (int y = 0; y < DIM; y++)
        for (int x = 0; x < DIM; x++)
            img[y][x] = 255;

    for (int u = 0; u < N; u++)
        for (int v = 0; v < N; v++)
            for (int m = 0; m < N; m++)
                for (int n = 0; n < N; n++) {
                    double b = basis(u, v, m, n);       /* in [-1, 1] */
                    int g = (int) ((b + 1.0) * 0.5 * 255.0 + 0.5);
                    if (g < 0) g = 0; if (g > 255) g = 255;
                    int y = u * TILE + 1 + m;
                    int x = v * TILE + 1 + n;
                    img[y][x] = (unsigned char) g;
                }

    FILE *f = fopen("dct_basis.pgm", "wb");
    if (!f) { perror("fopen"); return 1; }
    fprintf(f, "P5\n%d %d\n255\n", DIM, DIM);
    fwrite(img, 1, (size_t) DIM * DIM, f);
    fclose(f);

    printf("Wrote dct_basis.pgm (%dx%d): the 64 DCT basis images.\n", DIM, DIM);
    return 0;
}
