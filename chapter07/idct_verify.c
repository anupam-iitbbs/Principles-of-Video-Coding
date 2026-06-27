/*
 * idct_verify.c -- Inverse 2-D DCT and a perfect-reconstruction check.
 * Companion code for Chapter 7, "Transform Coding".
 *
 * The forward and inverse DCT are exact inverses (up to floating-point
 * rounding). This program transforms a block, inverts it, and reports the
 * maximum reconstruction error -- which should be essentially zero. Any
 * real loss in a codec comes from quantization, not from the transform.
 */
#include <stdio.h>
#include <math.h>

#define N 8

static double alpha(int k) { return (k == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N); }

static void dct_1d(const double in[N], double out[N])
{
    for (int k = 0; k < N; k++) {
        double s = 0.0;
        for (int n = 0; n < N; n++)
            s += in[n] * cos(M_PI * (2 * n + 1) * k / (2.0 * N));
        out[k] = alpha(k) * s;
    }
}

static void idct_1d(const double in[N], double out[N])
{
    for (int n = 0; n < N; n++) {
        double s = 0.0;
        for (int k = 0; k < N; k++)
            s += alpha(k) * in[k] * cos(M_PI * (2 * n + 1) * k / (2.0 * N));
        out[n] = s;
    }
}

static void apply_2d(void (*t)(const double[N], double[N]),
                     double src[N][N], double dst[N][N])
{
    double tmp[N][N], row[N], out[N];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) row[j] = src[i][j];
        t(row, out);
        for (int j = 0; j < N; j++) tmp[i][j] = out[j];
    }
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) row[i] = tmp[i][j];
        t(row, out);
        for (int i = 0; i < N; i++) dst[i][j] = out[i];
    }
}

int main(void)
{
    double block[N][N], coeff[N][N], recon[N][N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            block[i][j] = (i * 13 + j * 7) % 256;

    apply_2d(dct_1d,  block, coeff);
    apply_2d(idct_1d, coeff, recon);

    double max_err = 0.0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            double e = fabs(block[i][j] - recon[i][j]);
            if (e > max_err) max_err = e;
        }

    printf("Maximum reconstruction error: %.3e\n", max_err);
    printf("(Essentially zero -> the DCT is perfectly reversible.)\n");
    return 0;
}
