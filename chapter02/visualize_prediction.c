/*
 * visualize_prediction.c  --  Volume 2, Chapter 2: Intra Prediction
 *
 * Apply per-block intra prediction across a grayscale image and write out
 * three PGM files: the original, the prediction, and the residual (with a
 * mid-gray bias so zero residual appears as gray 128). Reads a raw 8-bit
 * grayscale image (width and height given on the command line).
 *
 * For each non-border block the encoder tries DC, vertical and horizontal
 * prediction and keeps the one with the smallest SAD -- a miniature mode
 * decision. Border blocks fall back to DC using whatever samples exist.
 *
 * Usage:  ./visualize_prediction input.raw width height
 * Build:  gcc -O2 -o visualize_prediction visualize_prediction.c
 */
#include <stdio.h>
#include <stdlib.h>

#define B 4   /* block size */

static int clamp8(int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); }

static void write_pgm(const char *name, const unsigned char *p, int w, int h)
{
    FILE *f = fopen(name, "wb");
    if (!f) { perror(name); return; }
    fprintf(f, "P5\n%d %d\n255\n", w, h);
    fwrite(p, 1, (size_t)w * h, f);
    fclose(f);
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s input.raw width height\n", argv[0]);
        return 1;
    }
    int w = atoi(argv[2]), h = atoi(argv[3]);
    long n = (long)w * h;

    unsigned char *img  = malloc(n);
    unsigned char *pred = malloc(n);
    unsigned char *res  = malloc(n);
    if (!img || !pred || !res) { fprintf(stderr, "alloc failed\n"); return 1; }

    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 1; }
    if (fread(img, 1, n, f) != (size_t)n) { fprintf(stderr, "short read\n"); return 1; }
    fclose(f);

    for (int by = 0; by < h; by += B) {
        for (int bx = 0; bx < w; bx += B) {
            /* gather references (fall back to 128 at the picture border) */
            int top[B], left[B];
            for (int k = 0; k < B; k++) {
                top[k]  = (by > 0 && bx + k < w) ? img[(by - 1) * w + bx + k] : 128;
                left[k] = (bx > 0 && by + k < h) ? img[(by + k) * w + bx - 1] : 128;
            }
            /* candidate predictions */
            int sum = 0;
            for (int k = 0; k < B; k++) sum += top[k] + left[k];
            int dc = (sum + B) / (2 * B);

            long sad_dc = 0, sad_v = 0, sad_h = 0;
            for (int i = 0; i < B; i++)
                for (int j = 0; j < B; j++) {
                    if (bx + j >= w || by + i >= h) continue;
                    int x = img[(by + i) * w + bx + j];
                    sad_dc += abs(x - dc);
                    sad_v  += abs(x - top[j]);
                    sad_h  += abs(x - left[i]);
                }
            int mode = 0; long best = sad_dc;       /* 0=DC */
            if (sad_v < best) { best = sad_v; mode = 1; }   /* 1=vertical */
            if (sad_h < best) { best = sad_h; mode = 2; }   /* 2=horizontal */

            for (int i = 0; i < B; i++)
                for (int j = 0; j < B; j++) {
                    if (bx + j >= w || by + i >= h) continue;
                    long off = (long)(by + i) * w + bx + j;
                    int p = (mode == 0) ? dc : (mode == 1) ? top[j] : left[i];
                    pred[off] = (unsigned char)clamp8(p);
                    res[off]  = (unsigned char)clamp8(img[off] - p + 128);
                }
        }
    }

    write_pgm("original.pgm",  img,  w, h);
    write_pgm("prediction.pgm", pred, w, h);
    write_pgm("residual.pgm",  res,  w, h);
    printf("wrote original.pgm, prediction.pgm, residual.pgm (%dx%d)\n", w, h);

    free(img); free(pred); free(res);
    return 0;
}
