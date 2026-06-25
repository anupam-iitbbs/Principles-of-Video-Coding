/*
 * best_mode.c  --  Volume 2, Chapter 2: Intra Prediction
 *
 * Simple intra mode decision. Evaluate DC, vertical, horizontal and one
 * angular candidate, then choose the mode with the lowest rate-distortion
 * cost  J = D + lambda * R, where D is the SSE of the residual and R is a
 * crude per-mode signaling cost in bits.
 *
 * Build: gcc -O2 -o best_mode best_mode.c
 */
#include <stdio.h>

#define N 4

enum { MODE_DC, MODE_VERT, MODE_HORIZ, MODE_ANG, NUM_MODES };
static const char *mode_name[] = { "DC", "Vertical", "Horizontal", "Angular" };

/* very rough estimate of bits to signal each mode */
static const int mode_bits[] = { 2, 2, 2, 5 };

static long sse(const int *img, const int *pred)
{
    long s = 0;
    for (int k = 0; k < N * N; k++) {
        int e = img[k] - pred[k];
        s += (long)e * e;
    }
    return s;
}

int main(void)
{
    int img[N * N] = {
        30, 70, 110, 150,
        32, 72, 108, 152,
        29, 69, 111, 149,
        31, 71, 110, 150
    };
    int top[N]  = { 30, 70, 110, 150 };
    int left[N] = { 30, 32, 29, 31 };
    int ref[2 * N] = { 30, 70, 110, 150, 150, 150, 150, 150 };

    int pred[NUM_MODES][N * N];

    /* DC */
    int sum = 0;
    for (int k = 0; k < N; k++) sum += top[k] + left[k];
    int dc = (sum + N) / (2 * N);
    for (int k = 0; k < N * N; k++) pred[MODE_DC][k] = dc;

    /* Vertical / Horizontal */
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            pred[MODE_VERT][i * N + j]  = top[j];
            pred[MODE_HORIZ][i * N + j] = left[i];
        }

    /* one angular candidate */
    int slope = 4;
    for (int i = 0; i < N; i++) {
        int offset = (i + 1) * slope;
        for (int j = 0; j < N; j++) {
            int pos = (j << 5) + offset, idx = pos >> 5, frac = pos & 31;
            pred[MODE_ANG][i * N + j] = ((32 - frac) * ref[idx] + frac * ref[idx + 1] + 16) >> 5;
        }
    }

    double lambda = 10.0;
    int best = 0;
    double bestJ = 1e18;
    printf("mode         D(SSE)   R(bits)        J\n");
    for (int m = 0; m < NUM_MODES; m++) {
        long D = sse(img, pred[m]);
        int  R = mode_bits[m];
        double J = (double)D + lambda * R;
        printf("%-10s  %7ld   %5d   %10.1f\n", mode_name[m], D, R, J);
        if (J < bestJ) { bestJ = J; best = m; }
    }
    printf("\nbest mode: %s  (J = %.1f)\n", mode_name[best], bestJ);
    return 0;
}
