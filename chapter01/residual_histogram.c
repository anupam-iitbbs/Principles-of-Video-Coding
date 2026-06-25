/*
 * residual_histogram.c
 * Volume 2, Chapter 1 - Prediction: The Missing Piece
 *
 * Computes and prints the histogram of an original image and
 * the histogram of its left-prediction residual. The original
 * histogram is spread across the whole 0..255 range; the
 * residual histogram is a sharp peak centred at zero. This is
 * the statistical reason residuals encode in fewer bits:
 * a peaked distribution has lower entropy.
 *
 * The histograms are written as two-column text files
 * (value count) suitable for plotting, and a compact ASCII
 * bar chart is printed to stdout.
 *
 * Build:  gcc -O2 -o residual_histogram residual_histogram.c -lm
 * Usage:  ./residual_histogram input.pgm
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

static double entropy(const long *hist, int bins, long total)
{
    double H = 0.0;
    for (int i = 0; i < bins; i++) {
        if (hist[i] == 0) continue;
        double p = (double)hist[i] / (double)total;
        H -= p * log2(p);
    }
    return H;
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

    long horig[256] = {0};
    long hres[512]  = {0};

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx  = y * w + x;
            horig[img[idx]]++;
            int pred = (x == 0) ? 128 : img[idx - 1];
            int r    = (int)img[idx] - pred;
            hres[r + 255]++;
        }
    }

    double Ho = entropy(horig, 256, n);
    double Hr = entropy(hres,  512, n);
    printf("original entropy : %.3f bits/sample\n", Ho);
    printf("residual entropy : %.3f bits/sample\n", Hr);
    printf("saving           : %.3f bits/sample\n", Ho - Hr);

    FILE *fo = fopen("hist_original.txt", "w");
    if (fo) { for (int i = 0; i < 256; i++) fprintf(fo, "%d %ld\n", i, horig[i]); fclose(fo); }
    FILE *fr = fopen("hist_residual.txt", "w");
    if (fr) { for (int i = 0; i < 512; i++) fprintf(fr, "%d %ld\n", i - 255, hres[i]); fclose(fr); }

    long peak = 0;
    for (int i = 0; i < 512; i++) if (hres[i] > peak) peak = hres[i];
    printf("\nresidual histogram near zero:\n");
    for (int r = -8; r <= 8; r++) {
        long c = hres[r + 255];
        int bars = (peak > 0) ? (int)((c * 40) / peak) : 0;
        printf("%4d | ", r);
        for (int b = 0; b < bars; b++) putchar('#');
        printf("\n");
    }

    free(img);
    return 0;
}
