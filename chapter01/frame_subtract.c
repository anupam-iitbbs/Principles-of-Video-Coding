/*
 * frame_subtract.c
 * Volume 2, Chapter 1 - Prediction: The Missing Piece
 *
 * Temporal prediction in its simplest form: predict the
 * current frame by the previous frame and code the residual
 *
 *     residual = current - previous
 *
 * For a talking-head clip most of the frame does not move, so
 * the residual is zero almost everywhere; only the moving
 * regions survive. The program reports how many residual
 * pixels are (near) zero -- a direct measure of temporal
 * redundancy -- and writes the difference image for viewing.
 *
 * Build:  gcc -O2 -o frame_subtract frame_subtract.c -lm
 * Usage:  ./frame_subtract prev.pgm curr.pgm diff.pgm [threshold]
 *
 * Both frames must have the same dimensions (8-bit P5 PGM).
 */
#include <stdio.h>
#include <stdlib.h>

static unsigned char *read_pgm(const char *path, int *w, int *h)
{
    FILE *f = fopen(path, "rb");
    if (!f) { perror("fopen"); return NULL; }
    char magic[3] = {0};
    if (fscanf(f, "%2s", magic) != 1 || magic[0] != 'P' || magic[1] != '5') {
        fprintf(stderr, "not a P5 PGM\n"); fclose(f); return NULL;
    }
    int maxval;
    if (fscanf(f, "%d %d %d", w, h, &maxval) != 3) {
        fprintf(stderr, "bad header\n"); fclose(f); return NULL;
    }
    fgetc(f);
    int n = (*w) * (*h);
    unsigned char *img = malloc(n);
    if (!img) { fclose(f); return NULL; }
    if (fread(img, 1, n, f) != (size_t)n) {
        fprintf(stderr, "short read\n"); free(img); fclose(f); return NULL;
    }
    fclose(f);
    return img;
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "usage: %s prev.pgm curr.pgm diff.pgm [threshold]\n", argv[0]);
        return 1;
    }
    int thr = (argc >= 5) ? atoi(argv[4]) : 2;

    int w1, h1, w2, h2;
    unsigned char *prev = read_pgm(argv[1], &w1, &h1);
    unsigned char *curr = read_pgm(argv[2], &w2, &h2);
    if (!prev || !curr) { free(prev); free(curr); return 1; }
    if (w1 != w2 || h1 != h2) {
        fprintf(stderr, "frame size mismatch\n");
        free(prev); free(curr); return 1;
    }

    int n = w1 * h1;
    FILE *f = fopen(argv[3], "wb");
    if (!f) { perror("fopen"); free(prev); free(curr); return 1; }
    fprintf(f, "P5\n%d %d\n255\n", w1, h1);

    long zeros = 0;
    double energy = 0.0;
    for (int i = 0; i < n; i++) {
        int r = (int)curr[i] - (int)prev[i];
        if ((r < 0 ? -r : r) <= thr) zeros++;
        energy += (double)r * r;
        int v = r + 128;
        if (v < 0) v = 0; if (v > 255) v = 255;
        fputc((unsigned char)v, f);
    }
    fclose(f);

    printf("frame size           : %d x %d (%d pixels)\n", w1, h1, n);
    printf("near-zero residuals  : %ld (%.1f%%) within +/-%d\n",
           zeros, 100.0 * zeros / n, thr);
    printf("residual energy/pixel: %.2f\n", energy / n);
    printf("wrote difference image to %s\n", argv[3]);

    free(prev); free(curr);
    return 0;
}
