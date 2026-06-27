/*
 * energy_compare.c -- Compare the energy distribution of a block before
 * and after the DCT. Companion code for Chapter 7, "Transform Coding".
 *
 * Energy compaction: a smooth block spreads its energy across all 64
 * pixel samples, but after the DCT the same energy collects into a few
 * low-frequency coefficients. This program prints the fraction of total
 * energy held by the largest k coefficients, for k = 1, 2, 4, 8.
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

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

static void dct_2d(double in[N][N], double out[N][N])
{
    double tmp[N][N], row[N], o[N];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) row[j] = in[i][j];
        dct_1d(row, o);
        for (int j = 0; j < N; j++) tmp[i][j] = o[j];
    }
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) row[i] = tmp[i][j];
        dct_1d(row, o);
        for (int i = 0; i < N; i++) out[i][j] = o[i];
    }
}

static int cmp_desc(const void *a, const void *b)
{
    double x = *(const double *) a, y = *(const double *) b;
    return (x < y) - (x > y);
}

int main(void)
{
    /* gentle gradient + slight texture */
    double block[N][N], coeff[N][N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            block[i][j] = 40.0 + 6.0 * i + 9.0 * j + ((i + j) % 2);

    dct_2d(block, coeff);

    /* squared magnitudes of the 64 coefficients, sorted descending */
    double e[N * N], total = 0.0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            double c = coeff[i][j];
            e[i * N + j] = c * c;
            total += c * c;
        }
    qsort(e, N * N, sizeof(double), cmp_desc);

    int marks[] = {1, 2, 4, 8};
    printf("Energy captured by the largest k DCT coefficients:\n\n");
    for (int m = 0; m < 4; m++) {
        double acc = 0.0;
        for (int t = 0; t < marks[m]; t++) acc += e[t];
        printf("  k = %d : %6.2f%% of total energy\n",
               marks[m], 100.0 * acc / total);
    }
    printf("\nA handful of coefficients hold nearly all the energy.\n");
    return 0;
}
