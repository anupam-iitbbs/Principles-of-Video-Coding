/*
 * coefficient_heatmap.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * Transforms an 8x8 block and prints an ASCII heatmap of coefficient
 * magnitudes, making energy compaction toward the low-frequency
 * (top-left) corner visible at a glance.
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

static void dct2d(double in[N][N], double out[N][N]) {
    double tmp[N][N], a[N], b[N];
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) a[j] = in[i][j];
        dct1d(a, b);
        for (int j = 0; j < N; ++j) tmp[i][j] = b[j];
    }
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < N; ++i) a[i] = tmp[i][j];
        dct1d(a, b);
        for (int i = 0; i < N; ++i) out[i][j] = b[i];
    }
}

int main(void) {
    /* A smooth gradient block: a typical compressible residual. */
    double block[N][N], coeff[N][N];
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) block[i][j] = 40.0 + 2.0 * (i + j);

    dct2d(block, coeff);

    /* Find the peak magnitude for scaling. */
    double peak = 0.0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (fabs(coeff[i][j]) > peak) peak = fabs(coeff[i][j]);

    const char *ramp = " .:-=+*#@";
    printf("Coefficient-magnitude heatmap:\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int level = (int)(8.0 * fabs(coeff[i][j]) / (peak + 1e-12));
            if (level > 8) level = 8;
            printf("%c", ramp[level]);
        }
        printf("\n");
    }
    return 0;
}
