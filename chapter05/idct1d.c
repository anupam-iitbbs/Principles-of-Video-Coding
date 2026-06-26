/*
 * idct1d.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * Inverse 1-D DCT (Equation 5.6). Demonstrates that the forward DCT
 * followed by the inverse DCT reproduces the input to within
 * floating-point error: the transform itself is lossless.
 */

#include <stdio.h>
#include <math.h>

#define N 8

void dct1d(const double *x, double *X, int n) {
    for (int u = 0; u < n; ++u) {
        double alpha = (u == 0) ? sqrt(1.0 / n) : sqrt(2.0 / n);
        double sum = 0.0;
        for (int i = 0; i < n; ++i)
            sum += x[i] * cos((2.0 * i + 1.0) * u * M_PI / (2.0 * n));
        X[u] = alpha * sum;
    }
}

/* Inverse DCT: x[i] = sum_u alpha(u) X[u] cos[(2i+1) u pi / 2N]. */
void idct1d(const double *X, double *x, int n) {
    for (int i = 0; i < n; ++i) {
        double sum = 0.0;
        for (int u = 0; u < n; ++u) {
            double alpha = (u == 0) ? sqrt(1.0 / n) : sqrt(2.0 / n);
            sum += alpha * X[u] * cos((2.0 * i + 1.0) * u * M_PI / (2.0 * n));
        }
        x[i] = sum;
    }
}

int main(void) {
    double x[N] = { 17, 22, 19, 25, 30, 28, 24, 20 };
    double X[N], xr[N];

    dct1d(x, X, N);
    idct1d(X, xr, N);

    double maxerr = 0.0;
    for (int i = 0; i < N; ++i) {
        double e = fabs(x[i] - xr[i]);
        if (e > maxerr) maxerr = e;
    }
    printf("max reconstruction error = %.3e (should be ~0)\n", maxerr);
    return 0;
}
