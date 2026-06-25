/*
 * left_predict.c
 * Volume 2, Chapter 1 - Prediction: The Missing Piece
 *
 * Left (horizontal) prediction on a grayscale image.
 * Each pixel is predicted from its immediate left neighbour;
 * the first column is predicted from a fixed value (128).
 * The residual r = x - xhat is written out and basic
 * statistics (mean, variance) are reported so the reader can
 * see that the residual has far less energy than the original.
 *
 * Build:  gcc -O2 -o left_predict left_predict.c -lm
 * Usage:  ./left_predict input.pgm residual.pgm
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

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s input.pgm residual.pgm\n", argv[0]);
        return 1;
    }
    int w, h;
    unsigned char *img = read_pgm(argv[1], &w, &h);
    if (!img) return 1;

    int n = w * h;
    int *res = malloc(n * sizeof(int));
    if (!res) { free(img); return 1; }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx  = y * w + x;
            int pred = (x == 0) ? 128 : img[idx - 1];
            res[idx] = (int)img[idx] - pred;
        }
    }

    double mo = 0, mr = 0;
    for (int i = 0; i < n; i++) { mo += img[i]; mr += res[i]; }
    mo /= n; mr /= n;
    double vo = 0, vr = 0;
    for (int i = 0; i < n; i++) {
        double a = img[i] - mo; vo += a * a;
        double b = res[i] - mr; vr += b * b;
    }
    vo /= n; vr /= n;

    printf("original : mean %.2f  variance %.2f\n", mo, vo);
    printf("residual : mean %.2f  variance %.2f\n", mr, vr);
    if (vr > 0.0)
        printf("prediction gain : %.2f dB\n", 10.0 * log10(vo / vr));

    if (write_pgm_shifted(argv[2], res, w, h) == 0)
        printf("wrote residual image to %s\n", argv[2]);

    free(res);
    free(img);
    return 0;
}
