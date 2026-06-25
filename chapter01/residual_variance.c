/*
 * residual_variance.c
 * Volume 2, Chapter 1 - Prediction: The Missing Piece
 *
 * Measures the variance of an image and the variance of its
 * left-prediction residual, then reports the prediction gain
 *
 *     G = 10 * log10( var_x / var_e )   [dB]
 *
 * It also estimates the lag-one horizontal correlation
 * coefficient rho and verifies the theoretical relation
 *
 *     var_e ~= var_x * (1 - rho^2)
 *
 * which links the statistics of Volume 1 to the prediction
 * of Volume 2: the more correlated the samples, the smaller
 * the residual.
 *
 * Build:  gcc -O2 -o residual_variance residual_variance.c -lm
 * Usage:  ./residual_variance input.pgm
 *
 * Input is an 8-bit binary PGM (P5).
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

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s input.pgm\n", argv[0]);
        return 1;
    }
    int w, h;
    unsigned char *img = read_pgm(argv[1], &w, &h);
    if (!img) return 1;
    int n = w * h;

    double mx = 0.0;
    for (int i = 0; i < n; i++) mx += img[i];
    mx /= n;
    double vx = 0.0;
    for (int i = 0; i < n; i++) { double d = img[i] - mx; vx += d * d; }
    vx /= n;

    double me = 0.0; long cnt = 0;
    for (int y = 0; y < h; y++)
        for (int x = 1; x < w; x++) { me += (int)img[y*w+x] - (int)img[y*w+x-1]; cnt++; }
    me /= cnt;
    double ve = 0.0;
    for (int y = 0; y < h; y++)
        for (int x = 1; x < w; x++) {
            double r = ((int)img[y*w+x] - (int)img[y*w+x-1]) - me;
            ve += r * r;
        }
    ve /= cnt;

    double cov = 0.0;
    for (int y = 0; y < h; y++)
        for (int x = 1; x < w; x++)
            cov += (img[y*w+x] - mx) * (img[y*w+x-1] - mx);
    cov /= cnt;
    double rho = (vx > 0.0) ? cov / vx : 0.0;

    printf("image variance     var_x : %.2f\n", vx);
    printf("residual variance  var_e : %.2f\n", ve);
    printf("lag-1 correlation  rho   : %.4f\n", rho);
    printf("predicted var_e = var_x*(1-rho^2) : %.2f\n", vx * (1.0 - rho * rho));
    if (ve > 0.0)
        printf("prediction gain          : %.2f dB\n", 10.0 * log10(vx / ve));

    free(img);
    return 0;
}
