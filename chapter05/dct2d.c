/*
 * dct2d.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * Separable 2-D DCT on an 8x8 block (Equation 5.7): a 1-D DCT along
 * each row, followed by a 1-D DCT down each column. Energy collects
 * in the top-left (low-frequency) corner.
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

/* Separable 2-D DCT: rows first, then columns. */
void dct2d(double in[N][N], double out[N][N]) {
    double tmp[N][N];
    double row[N], res[N];

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) row[j] = in[i][j];
        dct1d(row, res, N);
        for (int j = 0; j < N; ++j) tmp[i][j] = res[j];
    }
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < N; ++i) row[i] = tmp[i][j];
        dct1d(row, res, N);
        for (int i = 0; i < N; ++i) out[i][j] = res[i];
    }
}

int main(void) {
    /* A flat block: only the DC coefficient should survive. */
    double block[N][N], coeff[N][N];
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) block[i][j] = 50.0;

    dct2d(block, coeff);

    printf("2-D DCT coefficients (DC at top-left):\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) printf("% 8.2f", coeff[i][j]);
        printf("\n");
    }
    return 0;
}
