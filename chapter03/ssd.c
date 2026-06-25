/*
 * ssd.c
 * Sum of Squared Differences (SSD) between two equally sized blocks.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * SSD = sum (x(i,j) - r(i,j))^2.  Because it squares each difference, it
 * penalises a few large errors more heavily than many small ones, and it
 * equals the energy of the residual.  It is a better predictor of true
 * coding cost than SAD, at the price of the multiplications.
 */

#include <stdio.h>

/* Sum of Squared Differences between blocks x and r (rows x cols, row-major). */
long block_ssd(const int *x, const int *r, int rows, int cols)
{
    long ssd = 0;
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            int d = x[i * cols + j] - r[i * cols + j];
            ssd += (long)d * d;
        }
    }
    return ssd;
}

int main(void)
{
    int x[4] = { 50, 52, 49, 51 };
    int r[4] = { 50, 50, 50, 50 };

    long ssd = block_ssd(x, r, 2, 2);
    printf("SSD = %ld\n", ssd);   /* expected: 6 */
    return 0;
}
