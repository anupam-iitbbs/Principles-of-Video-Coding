/*
 * top_predict.c
 * Volume 2, Chapter 1 - Prediction: The Missing Piece
 *
 * Top (vertical) prediction on a grayscale image.
 * Each pixel is predicted from the pixel directly above it;
 * the first row is predicted from a fixed value (128).
 *
 * For comparison, the program also computes the residual
 * energy of LEFT prediction, so the reader can see which
 * direction better matches the image content. The winner
 * depends on whether the image has more horizontal or more
 * vertical structure -- a first taste of mode decision.
 *
 * Build:  gcc -O2 -o top_predict top_predict.c -lm
 * Usage:  ./top_predict input.pgm residual_top.pgm
 *
 * Input/output are 8-bit binary PGM (P5).
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

static int write_pgm_shifted(const char *path, const int *res, int w, int h)
{
    FILE *f = fopen(path, "wb");
    if (!f) { perror("fopen"); return -1; }
    fprintf(f, "P5\n%d %d\n255\n", w, h);
    for (int i = 0; i < w * h; i++) {
        int v = res[i] + 128;
        if (v < 0) v = 0; if (v > 255) v = 255;
        fputc((unsigned char)v, f);
    }
    fclose(f);
    return 0;
}

static double residual_energy(const int *res, int n)
{
    double m = 0.0;
    for (int i = 0; i < n; i++) m += res[i];
    m /= n;
    double e = 0.0;
    for (int i = 0; i < n; i++) { double d = res[i] - m; e += d * d; }
    return e;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s input.pgm residual_top.pgm\n", argv[0]);
        return 1;
    }
    int w, h;
    unsigned char *img = read_pgm(argv[1], &w, &h);
    if (!img) return 1;

    int n = w * h;
    int *top  = malloc(n * sizeof(int));
    int *left = malloc(n * sizeof(int));
    if (!top || !left) { free(img); free(top); free(left); return 1; }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = y * w + x;
            int pt  = (y == 0) ? 128 : img[idx - w];
            int pl  = (x == 0) ? 128 : img[idx - 1];
            top[idx]  = (int)img[idx] - pt;
            left[idx] = (int)img[idx] - pl;
        }
    }

    double et = residual_energy(top,  n);
    double el = residual_energy(left, n);
    printf("top  prediction residual energy : %.0f\n", et);
    printf("left prediction residual energy : %.0f\n", el);
    printf("better direction : %s\n", (et < el) ? "TOP" : "LEFT");

    if (write_pgm_shifted(argv[2], top, w, h) == 0)
        printf("wrote top-prediction residual to %s\n", argv[2]);

    free(top); free(left); free(img);
    return 0;
}
