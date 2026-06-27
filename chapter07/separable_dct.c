/*
 * separable_dct.c -- Compute the 8x8 DCT two ways and compare. Companion
 * code for Chapter 7, "Transform Coding".
 *
 * Direct 2-D DCT:    sum over a 4-D nest -> O(N^4) per block.
 * Separable DCT:     rows then columns   -> O(N^3) per block.
 *
 * Both produce the same result (to rounding); the separable form does far
 * fewer multiply-adds, which is why every real codec uses it.
 */
#include <stdio.h>
#include <math.h>

#define N 8

static double alpha(int k) { return (k == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N); }

/* Direct 2-D DCT straight from the double sum definition. */
static long dct_direct(double in[N][N], double out[N][N])
{
    long ops = 0;
    for (int u = 0; u < N; u++)
        for (int v = 0; v < N; v++) {
            double s = 0.0;
            for (int m = 0; m < N; m++)
                for (int n = 0; n < N; n++) {
                    s += in[m][n] *
                         cos(M_PI * (2 * m + 1) * u / (2.0 * N)) *
                         cos(M_PI * (2 * n + 1) * v / (2.0 * N));
                    ops++;
                }
            out[u][v] = alpha(u) * alpha(v) * s;
        }
    return ops;
}

static void dct_1d(const double in[N], double out[N], long *ops)
{
    for (int k = 0; k < N; k++) {
        double s = 0.0;
        for (int n = 0; n < N; n++) { s += in[n] * cos(M_PI*(2*n+1)*k/(2.0*N)); (*ops)++; }
        out[k] = alpha(k) * s;
    }
}

/* Separable: rows then columns. */
static long dct_separable(double in[N][N], double out[N][N])
{
    long ops = 0;
    double tmp[N][N], row[N], o[N];
    for (int i = 0; i < N; i++) { for(int j=0;j<N;j++) row[j]=in[i][j]; dct_1d(row,o,&ops); for(int j=0;j<N;j++) tmp[i][j]=o[j]; }
    for (int j = 0; j < N; j++) { for(int i=0;i<N;i++) row[i]=tmp[i][j]; dct_1d(row,o,&ops); for(int i=0;i<N;i++) out[i][j]=o[i]; }
    return ops;
}

int main(void)
{
    double in[N][N], a[N][N], b[N][N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            in[i][j] = (i * 11 + j * 5) % 100;

    long od = dct_direct(in, a);
    long os = dct_separable(in, b);

    double max_diff = 0.0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            double d = fabs(a[i][j] - b[i][j]);
            if (d > max_diff) max_diff = d;
        }

    printf("Max difference (direct vs separable): %.3e\n", max_diff);
    printf("Cosine multiply-adds: direct = %ld, separable = %ld\n", od, os);
    printf("Same answer, far fewer operations.\n");
    return 0;
}
