/*
 * idct2d.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * Separable inverse 2-D DCT. Transforms a block forward then inverse
 * and reports the maximum reconstruction error, confirming the 2-D
 * transform is a lossless round trip in exact arithmetic.
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

static void apply_rows_cols(double in[N][N], double out[N][N],
                            void (*op1d)(const double *, double *)) {
    double tmp[N][N], a[N], b[N];
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) a[j] = in[i][j];
        op1d(a, b);
        for (int j = 0; j < N; ++j) tmp[i][j] = b[j];
    }
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < N; ++i) a[i] = tmp[i][j];
        op1d(a, b);
        for (int i = 0; i < N; ++i) out[i][j] = b[i];
    }
}

int main(void) {
    double block[N][N], coeff[N][N], recon[N][N];
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) block[i][j] = (i * 7 + j * 3) % 64;

    apply_rows_cols(block, coeff, dct1d);
    apply_rows_cols(coeff, recon, idct1d);

    double maxerr = 0.0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            double e = fabs(block[i][j] - recon[i][j]);
            if (e > maxerr) maxerr = e;
        }
    printf("max 2-D reconstruction error = %.3e (should be ~0)\n", maxerr);
    return 0;
}
