/*
 * truncate_reconstruct.c -- Reconstruct an 8x8 block from only its
 * largest N coefficients. Companion code for Chapter 7, "Transform
 * Coding".
 *
 * Because the DCT compacts energy, keeping only a few coefficients
 * already reproduces most of the block. This program reconstructs from
 * the largest N = 1, 2, 4, 8, 16, 64 coefficients and reports the
 * resulting mean-squared error.
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
                     double s[N][N], double d[N][N])
{
    double tmp[N][N], row[N], o[N];
    for (int i = 0; i < N; i++) { for (int j=0;j<N;j++) row[j]=s[i][j]; t(row,o); for(int j=0;j<N;j++) tmp[i][j]=o[j]; }
    for (int j = 0; j < N; j++) { for (int i=0;i<N;i++) row[i]=tmp[i][j]; t(row,o); for(int i=0;i<N;i++) d[i][j]=o[i]; }
}

int main(void)
{
    double block[N][N], coeff[N][N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            block[i][j] = 50.0 + 8.0 * i + 5.0 * j;

    apply_2d(dct_1d, block, coeff);

    int keep[] = {1, 2, 4, 8, 16, 64};
    printf("Reconstruction from the largest N coefficients:\n\n");
    for (int m = 0; m < 6; m++) {
        /* threshold: find magnitude of the N-th largest coefficient */
        double mags[N * N];
        int p = 0;
        for (int i = 0; i < N; i++) for (int j = 0; j < N; j++) mags[p++] = fabs(coeff[i][j]);
        for (int a = 0; a < N * N; a++)
            for (int b = a + 1; b < N * N; b++)
                if (mags[b] > mags[a]) { double t = mags[a]; mags[a] = mags[b]; mags[b] = t; }
        double thresh = mags[keep[m] - 1];

        double kept[N][N], recon[N][N];
        for (int i = 0; i < N; i++) for (int j = 0; j < N; j++)
            kept[i][j] = (fabs(coeff[i][j]) >= thresh) ? coeff[i][j] : 0.0;

        apply_2d(idct_1d, kept, recon);

        double mse = 0.0;
        for (int i = 0; i < N; i++) for (int j = 0; j < N; j++) {
            double e = block[i][j] - recon[i][j];
            mse += e * e;
        }
        mse /= (N * N);
        printf("  N = %2d kept : MSE = %10.4f\n", keep[m], mse);
    }
    printf("\nError falls quickly: the first few coefficients matter most.\n");
    return 0;
}
