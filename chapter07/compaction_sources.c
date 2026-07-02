/*
 * compaction_sources.c -- Challenge exercise for Chapter 7, "Transform
 * Coding". Compares DCT energy compaction on three very different 8x8
 * blocks:
 *
 *   1. a smooth natural block (gentle gradient),
 *   2. a text-like block (sharp black/white transitions),
 *   3. a block of pure random noise.
 *
 * For each, it reports the fraction of total energy captured by the largest
 * few coefficients. The smooth block compacts dramatically, the text block
 * partially, and the noise block hardly at all -- which is exactly why
 * transforms compress natural images but not random data.
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
    for (int i=0;i<N;i++){for(int j=0;j<N;j++)row[j]=in[i][j];dct_1d(row,o);for(int j=0;j<N;j++)tmp[i][j]=o[j];}
    for (int j=0;j<N;j++){for(int i=0;i<N;i++)row[i]=tmp[i][j];dct_1d(row,o);for(int i=0;i<N;i++)out[i][j]=o[i];}
}

static int cmp_desc(const void *a, const void *b)
{
    double x = *(const double *) a, y = *(const double *) b;
    return (x < y) - (x > y);
}

/* fraction of energy in the largest k coefficients */
static double top_k_energy(double blk[N][N], int k)
{
    double coeff[N][N];
    dct_2d(blk, coeff);
    double e[N * N], total = 0.0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            double c = coeff[i][j];
            e[i * N + j] = c * c;
            total += c * c;
        }
    qsort(e, N * N, sizeof(double), cmp_desc);
    double acc = 0.0;
    for (int t = 0; t < k; t++) acc += e[t];
    return (total > 0.0) ? 100.0 * acc / total : 0.0;
}

int main(void)
{
    double smooth[N][N], text[N][N], noise[N][N];

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            smooth[i][j] = 40.0 + 6.0 * i + 8.0 * j;          /* gradient   */
            text[i][j]   = ((j / 2) % 2 == 0) ? 20.0 : 235.0; /* stripes    */
            noise[i][j]  = rand() % 256;                       /* white noise */
        }

    printf("Energy in the largest coefficients (%% of total):\n\n");
    printf("  source     top-4      top-8\n");
    printf("  smooth   %6.2f%%   %6.2f%%\n", top_k_energy(smooth, 4), top_k_energy(smooth, 8));
    printf("  text     %6.2f%%   %6.2f%%\n", top_k_energy(text, 4), top_k_energy(text, 8));
    printf("  noise    %6.2f%%   %6.2f%%\n", top_k_energy(noise, 4), top_k_energy(noise, 8));
    printf("\nSmooth compacts dramatically; noise barely compacts at all.\n");
    return 0;
}
