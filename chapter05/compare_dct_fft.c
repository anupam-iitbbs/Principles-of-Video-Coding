/*
 * compare_dct_fft.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * Compares the energy compaction of the DCT against the DFT (Fourier)
 * on the same smooth signal. The DFT assumes the block repeats, so its
 * boundary discontinuity spills energy into high frequencies; the DCT
 * reflects the block and compacts energy far better.
 */

#include <stdio.h>
#include <math.h>

#define N 8

static double alpha(int u) { return (u == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N); }

static void dct1d(const double *x, double *mag) {
    for (int u = 0; u < N; ++u) {
        double sum = 0.0;
        for (int i = 0; i < N; ++i)
            sum += x[i] * cos((2.0 * i + 1.0) * u * M_PI / (2.0 * N));
        mag[u] = fabs(alpha(u) * sum);
    }
}

static void dft_mag(const double *x, double *mag) {
    for (int k = 0; k < N; ++k) {
        double re = 0.0, im = 0.0;
        for (int n = 0; n < N; ++n) {
            double ang = -2.0 * M_PI * k * n / N;
            re += x[n] * cos(ang);
            im += x[n] * sin(ang);
        }
        mag[k] = sqrt(re * re + im * im);
    }
}

/* Fraction of total energy held by the first HALF of the coefficients. */
static double low_energy_fraction(const double *mag) {
    double total = 0.0, low = 0.0;
    for (int k = 0; k < N; ++k) {
        total += mag[k] * mag[k];
        if (k < N / 2) low += mag[k] * mag[k];
    }
    return low / total;
}

int main(void) {
    /* A smooth ramp with non-matching endpoints (worst case for DFT). */
    double x[N] = { 10, 14, 18, 22, 26, 30, 34, 38 };
    double dmag[N], fmag[N];

    dct1d(x, dmag);
    dft_mag(x, fmag);

    printf("Low-frequency energy fraction (first %d coefficients):\n", N / 2);
    printf("  DCT: %.4f\n", low_energy_fraction(dmag));
    printf("  DFT: %.4f\n", low_energy_fraction(fmag));
    printf("Higher fraction means better energy compaction.\n");
    return 0;
}
