/*
 * dct_2d.c -- Separable 2-D DCT-II for an 8x8 block. Companion code for
 * Chapter 7, "Transform Coding".
 *
 * The 2-D DCT factors into 1-D transforms: transform every row, then
 * every column of the result. This is the "separable" form that real
 * codecs exploit for speed and simple hardware.
 */
#include <stdio.h>
#include <math.h>

#define N 8

/* Forward 1-D DCT-II */
static void dct_1d(const double in[N], double out[N])
{
    for (int k = 0; k < N; k++) {
        double sum = 0.0;
        for (int n = 0; n < N; n++)
            sum += in[n] * cos(M_PI * (2 * n + 1) * k / (2.0 * N));
        double a = (k == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);
        out[k] = a * sum;
    }
}

/* Separable 2-D DCT: rows then columns */
void dct_2d(double block[N][N], double coeff[N][N])
{
    double tmp[N][N], row[N], out[N];

    /* transform each row */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) row[j] = block[i][j];
        dct_1d(row, out);
        for (int j = 0; j < N; j++) tmp[i][j] = out[j];
    }
    /* transform each column of the intermediate result */
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) row[i] = tmp[i][j];
        dct_1d(row, out);
        for (int i = 0; i < N; i++) coeff[i][j] = out[i];
    }
}

int main(void)
{
    /* A smooth gradient block: brightness rises left to right. */
    double block[N][N], coeff[N][N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            block[i][j] = 30.0 + 10.0 * j;

    dct_2d(block, coeff);

    printf("2-D DCT coefficients of a horizontal gradient:\n\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("% 8.2f", coeff[i][j]);
        printf("\n");
    }
    printf("\nEnergy collects in the top-left (DC and low-frequency) corner.\n");
    return 0;
}
