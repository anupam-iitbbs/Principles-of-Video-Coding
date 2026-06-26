/*
 * parseval.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * Verifies Parseval theorem (Equation 5.3): for an orthonormal
 * transform the energy of the signal equals the energy of its
 * coefficients. Energy is moved, not lost.
 */

#include <stdio.h>
#include <math.h>

#define N 8

static void dct1d(const double *x, double *X, int n) {
    for (int u = 0; u < n; ++u) {
        double alpha = (u == 0) ? sqrt(1.0 / n) : sqrt(2.0 / n);
        double sum = 0.0;
        for (int i = 0; i < n; ++i)
            sum += x[i] * cos((2.0 * i + 1.0) * u * M_PI / (2.0 * n));
        X[u] = alpha * sum;
    }
}

static double energy(const double *v, int n) {
    double e = 0.0;
    for (int i = 0; i < n; ++i) e += v[i] * v[i];
    return e;
}

int main(void) {
    double x[N] = { 31, 27, 40, 22, 18, 35, 29, 24 };
    double X[N];

    dct1d(x, X, N);

    double ex = energy(x, N);
    double eX = energy(X, N);
    printf("signal energy      = %.6f\n", ex);
    printf("coefficient energy = %.6f\n", eX);
    printf("difference         = %.3e (should be ~0)\n", fabs(ex - eX));
    return 0;
}
