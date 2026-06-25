/*
 * difference_image.c
 * Volume 2, Chapter 1 - Prediction: The Missing Piece
 *
 * Computes the difference image between two consecutive video
 * frames: d = curr - prev, displayed with a +128 offset so
 * that zero (no change) maps to mid-grey. For a typical clip
 * the result is mostly grey, with structure only where motion
 * occurred -- a direct visualisation of temporal redundancy.
 *
 * Build:  gcc -O2 -o difference_image difference_image.c -lm
 * Usage:  ./difference_image prev.pgm curr.pgm diff.pgm
 *
 * Frames are 8-bit binary PGM (P5) of equal size.
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
    if (argc != 4) {
        fprintf(stderr, "usage: %s prev.pgm curr.pgm diff.pgm\n", argv[0]);
        return 1;
    }
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

    long changed = 0;
    for (int i = 0; i < n; i++) {
        int d = (int)curr[i] - (int)prev[i];
        if (d) changed++;
        int v = d + 128;
        if (v < 0) v = 0; if (v > 255) v = 255;
        fputc((unsigned char)v, f);
    }
    fclose(f);

    printf("changed pixels : %ld of %d (%.1f%%)\n",
           changed, n, 100.0 * changed / n);
    printf("wrote difference image to %s\n", argv[3]);

    free(prev); free(curr);
    return 0;
}
