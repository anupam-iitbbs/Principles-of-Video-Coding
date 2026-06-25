/*
 * visualize_residual.c
 * Volume 2, Chapter 1 - Prediction: The Missing Piece
 *
 * A raw residual is dominated by values near zero and looks
 * almost flat grey. This program rescales the residual so
 * that its structure (edges, moving regions) becomes clearly
 * visible. Two modes are offered:
 *
 *   - 'gain'      : r -> 128 + gain*r, clamped (linear stretch)
 *   - 'magnitude' : r -> scaled |r|   (bright where it changes)
 *
 * The residual itself is produced by left prediction so the
 * program is self-contained and needs only one input image.
 *
 * Build:  gcc -O2 -o visualize_residual visualize_residual.c -lm
 * Usage:  ./visualize_residual input.pgm out.pgm [gain|magnitude] [k]
 *
 * Input/output are 8-bit binary PGM (P5).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static int clamp(int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); }

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s input.pgm out.pgm [gain|magnitude] [k]\n", argv[0]);
        return 1;
    }
    const char *mode = (argc >= 4) ? argv[3] : "gain";
    double k = (argc >= 5) ? atof(argv[4]) : 4.0;
    int magnitude = (strcmp(mode, "magnitude") == 0);

    int w, h;
    unsigned char *img = read_pgm(argv[1], &w, &h);
    if (!img) return 1;

    FILE *f = fopen(argv[2], "wb");
    if (!f) { perror("fopen"); free(img); return 1; }
    fprintf(f, "P5\n%d %d\n255\n", w, h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx  = y * w + x;
            int pred = (x == 0) ? 128 : img[idx - 1];
            int r    = (int)img[idx] - pred;
            int v;
            if (magnitude) {
                int a = r < 0 ? -r : r;
                v = clamp((int)(k * a));
            } else {
                v = clamp(128 + (int)(k * r));
            }
            fputc((unsigned char)v, f);
        }
    }
    fclose(f);

    printf("mode %s, gain %.1f -> wrote %s\n", mode, k, argv[2]);
    printf("tip: increase k to make faint prediction error visible\n");

    free(img);
    return 0;
}
