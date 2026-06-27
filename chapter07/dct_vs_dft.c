/*
 * dct_vs_dft.c -- Compare how the DCT and the DFT spread the energy of a
 * 1-D signal. Companion code for Chapter 7, "Transform Coding".
 *
 * The DFT treats a finite block as one period of a repeating signal. If
 * the left and right ends differ, the implied wrap-around creates a jump,
 * which injects spurious high-frequency energy. The DCT assumes mirror
 * symmetry, so a smooth-but-non-periodic signal stays smooth -- and its
 * energy compacts far better.
 */
#include <stdio.h>
#include <math.h>

#define N 8

static double alpha(int k) { return (k == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N); }

static void dct(const double x[N], double mag[N])
{
    for (int k = 0; k < N; k++) {
        double s = 0.0;
        for (int n = 0; n < N; n++)
            s += x[n] * cos(M_PI * (2 * n + 1) * k / (2.0 * N));
        mag[k] = fabs(alpha(k) * s);
    }
}

static void dft(const double x[N], double mag[N])
{
    for (int k = 0; k < N; k++) {
        double re = 0.0, im = 0.0;
        for (int n = 0; n < N; n++) {
            double ang = -2.0 * M_PI * k * n / N;
            re += x[n] * cos(ang);
            im += x[n] * sin(ang);
        }
        mag[k] = sqrt(re * re + im * im) / sqrt((double) N);
    }
}

int main(void)
{
    /* A ramp: smooth but NOT periodic (ends differ sharply). */
    double x[N];
    for (int n = 0; n < N; n++) x[n] = (double) n;

    double md[N], mf[N];
    dct(x, md);
    dft(x, mf);

    printf("Magnitude spectra of a ramp (smooth, non-periodic):\n\n");
    printf("  k     DCT        DFT\n");
    for (int k = 0; k < N; k++)
        printf(" %2d  %8.3f   %8.3f\n", k, md[k], mf[k]);

    /* energy beyond the first two bins, as a fraction of total */
    double td = 0, hd = 0, tf = 0, hf = 0;
    for (int k = 0; k < N; k++) {
        td += md[k] * md[k]; tf += mf[k] * mf[k];
        if (k >= 2) { hd += md[k] * md[k]; hf += mf[k] * mf[k]; }
    }
    printf("\nHigh-frequency energy (k>=2):\n");
    printf("  DCT: %5.2f%%   DFT: %5.2f%%\n", 100.0 * hd / td, 100.0 * hf / tf);
    printf("The DFT leaks more energy to high frequencies due to the edge jump.\n");
    return 0;
}
