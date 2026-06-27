/*
 * zero_ac.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * Transforms a block, zeros the high-frequency coefficients, inverse-
 * transforms, and measures the reconstruction error. Shows that most
 * of the high-frequency coefficients can be dropped almost for free.
 */

#include <stdio.h>
#include <math.h>

#define N 8

static double alpha(int u) { return (u == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N); }

static void dct1d(const double *x, double *X) {
    for (int u = 0; u < N; ++u) {
        double sum = 0.0;
        for (int i = 0; i < N; ++i)
            sum += x[i] * cos((2.0 * i + 1.0) * u * M_PI / (2.0 * N));
        X[u] = alpha(u) * sum;
    }
}

static void idct1d(const double *X, double *x) {
    for (int i = 0; i < N; ++i) {
        double sum = 0.0;
        for (int u = 0; u < N; ++u)
            sum += alpha(u) * X[u] * cos((2.0 * i + 1.0) * u * M_PI / (2.0 * N));
        x[i] = sum;
    }
}

static void pass(double in[N][N], double out[N][N],
                 void (*op)(const double *, double *)) {
    double tmp[N][N], a[N], b[N];
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) a[j] = in[i][j];
        op(a, b);
        for (int j = 0; j < N; ++j) tmp[i][j] = b[j];
    }
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < N; ++i) a[i] = tmp[i][j];
        op(a, b);
        for (int i = 0; i < N; ++i) out[i][j] = b[i];
    }
}

int main(void) {
    double block[N][N], coeff[N][N], recon[N][N];
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) block[i][j] = 64.0 + 3.0 * (i + j);

    pass(block, coeff, dct1d);

    /* Keep only coefficients where u + v < KEEP; zero the rest. */
    int KEEP = 3;
    int kept = 0;
    for (int u = 0; u < N; ++u)
        for (int v = 0; v < N; ++v)
            if (u + v >= KEEP) coeff[u][v] = 0.0; else kept++;

    pass(coeff, recon, idct1d);

    double maxerr = 0.0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            double e = fabs(block[i][j] - recon[i][j]);
            if (e > maxerr) maxerr = e;
        }
    printf("kept %d of %d coefficients\n", kept, N * N);
    printf("max reconstruction error = %.4f\n", maxerr);
    return 0;
}
