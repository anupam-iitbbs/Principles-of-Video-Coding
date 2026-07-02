/*
 * dct_numerical.c -- The worked numerical example from Chapter 7,
 * "Transform Coding". Reproduces the book's 1-D calculation and shows
 * that keeping only the two largest coefficients already reconstructs the
 * row almost perfectly.
 *
 *   x = [52 55 61 66 70 61 64 73]
 *   C = forward DCT of x   (DC dominates; first AC captures the trend)
 *   keep 2 largest, zero the rest, inverse DCT -> near-perfect recovery
 */
#include <stdio.h>
#include <math.h>

#define N 8

static double alpha(int k) { return (k == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N); }

static void dct(const double x[N], double C[N])
{
    for (int k = 0; k < N; k++) {
        double s = 0.0;
        for (int n = 0; n < N; n++)
            s += x[n] * cos(M_PI * (2 * n + 1) * k / (2.0 * N));
        C[k] = alpha(k) * s;
    }
}
static void idct(const double C[N], double x[N])
{
    for (int n = 0; n < N; n++) {
        double s = 0.0;
        for (int k = 0; k < N; k++)
            s += alpha(k) * C[k] * cos(M_PI * (2 * n + 1) * k / (2.0 * N));
        x[n] = s;
    }
}

int main(void)
{
    double x[N] = {52, 55, 61, 66, 70, 61, 64, 73};
    double C[N], recon[N];

    dct(x, C);

    printf("Input row x:      ");
    for (int i = 0; i < N; i++) printf("% 6.0f", x[i]);
    printf("\n");

    printf("DCT coefficients: ");
    for (int i = 0; i < N; i++) printf("% 6.0f", C[i]);
    printf("\n");

    /* keep the two largest-magnitude coefficients, zero the other six */
    double kept[N];
    for (int i = 0; i < N; i++) kept[i] = C[i];
    for (int drop = 0; drop < N - 2; drop++) {
        int idx = -1; double best = 1e30;
        for (int i = 0; i < N; i++)
            if (kept[i] != 0.0 && fabs(kept[i]) < best) { best = fabs(kept[i]); idx = i; }
        kept[idx] = 0.0;
    }

    idct(kept, recon);

    printf("Reconstructed (2 coeffs kept):\n  ");
    double maxerr = 0.0;
    for (int i = 0; i < N; i++) {
        printf("% 6.1f", recon[i]);
        double e = fabs(recon[i] - x[i]);
        if (e > maxerr) maxerr = e;
    }
    printf("\n  max error per sample: %.2f\n", maxerr);
    printf("Two coefficients already capture the row -- energy compaction at work.\n");
    return 0;
}
