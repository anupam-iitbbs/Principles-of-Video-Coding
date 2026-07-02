/*
 * integer_dct.c -- A 4x4 integer transform in the style of H.264, computed
 * with additions and shifts only. Companion code for Chapter 7,
 * "Transform Coding".
 *
 * The exact DCT uses irrational cosines that different machines round
 * differently, causing decoder drift across predicted frames. Replacing
 * the cosines with a fixed small-integer matrix makes the transform
 * bit-exact on every device. Here we run the classic butterfly form and
 * compare it against a floating-point 4x4 DCT of the same block.
 */
#include <stdio.h>
#include <math.h>

#define N 4

/* H.264-style integer forward transform of one row (butterfly form). */
static void int_row(const int in[N], int out[N])
{
    int s0 = in[0] + in[3];
    int s1 = in[1] + in[2];
    int s2 = in[1] - in[2];
    int s3 = in[0] - in[3];

    out[0] = s0 + s1;            /* 1  1  1  1 */
    out[1] = 2 * s3 + s2;        /* 2  1 -1 -2 */
    out[2] = s0 - s1;            /* 1 -1 -1  1 */
    out[3] = s3 - 2 * s2;        /* 1 -2  2 -1 */
}

/* Apply the integer transform separably: rows then columns. */
static void int_2d(int blk[N][N], int out[N][N])
{
    int tmp[N][N], row[N], o[N];
    for (int i = 0; i < N; i++) { for(int j=0;j<N;j++) row[j]=blk[i][j]; int_row(row,o); for(int j=0;j<N;j++) tmp[i][j]=o[j]; }
    for (int j = 0; j < N; j++) { for(int i=0;i<N;i++) row[i]=tmp[i][j]; int_row(row,o); for(int i=0;i<N;i++) out[i][j]=o[i]; }
}

/* Reference floating-point 4x4 DCT for comparison. */
static double alpha(int k) { return (k == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N); }
static void flt_2d(int blk[N][N], double out[N][N])
{
    for (int u = 0; u < N; u++)
        for (int v = 0; v < N; v++) {
            double s = 0.0;
            for (int m = 0; m < N; m++)
                for (int n = 0; n < N; n++)
                    s += blk[m][n] *
                         cos(M_PI * (2 * m + 1) * u / (2.0 * N)) *
                         cos(M_PI * (2 * n + 1) * v / (2.0 * N));
            out[u][v] = alpha(u) * alpha(v) * s;
        }
}

int main(void)
{
    int blk[N][N] = {
        { 5, 11, 8, 10 },
        { 9, 8, 4, 12 },
        { 1, 10, 11, 4 },
        { 19, 6, 15, 7 }
    };
    int ic[N][N];
    double fc[N][N];

    int_2d(blk, ic);
    flt_2d(blk, fc);

    printf("Integer transform (exact, bit-identical everywhere):\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) printf("% 6d", ic[i][j]);
        printf("\n");
    }
    printf("\nFloating-point DCT (for reference; note the scale differs):\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) printf("% 8.2f", fc[i][j]);
        printf("\n");
    }
    printf("\nThe integer version has no cosines, so every decoder agrees exactly.\n");
    return 0;
}
