/*
 * dct1d.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * Forward 1-D Discrete Cosine Transform (DCT-II) of a length-N signal,
 * implementing Equation (5.4) including the alpha(u) normalisation.
 */

#include <stdio.h>
#include <math.h>

#define N 8

/* Forward DCT: X[u] = alpha(u) * sum_n x[n] cos[(2n+1) u pi / 2N]. */
void dct1d(const double *x, double *X, int n) {
    for (int u = 0; u < n; ++u) {
        double alpha = (u == 0) ? sqrt(1.0 / n) : sqrt(2.0 / n);
        double sum = 0.0;
        for (int i = 0; i < n; ++i)
            sum += x[i] * cos((2.0 * i + 1.0) * u * M_PI / (2.0 * n));
        X[u] = alpha * sum;
    }
}

int main(void) {
    /* A smooth ramp: energy should concentrate in the low indices. */
    double x[N] = { 10, 12, 14, 16, 18, 20, 22, 24 };
    double X[N];

    dct1d(x, X, N);

    printf("DCT coefficients:\n");
    for (int u = 0; u < N; ++u)
        printf("  X[%d] = % .4f\n", u, X[u]);
    return 0;
}
