/*
 * sad.c
 * Sum of Absolute Differences (SAD) between two equally sized blocks.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * The SAD is the workhorse matching criterion of block-based motion
 * estimation:  SAD = sum |x(i,j) - r(i,j)|  over all pixels of the block.
 * It needs only subtractions, absolute values and additions, which makes
 * it cheap enough to run in the innermost loop of a full search.
 */

#include <stdio.h>
#include <stdlib.h>

/* Compute the Sum of Absolute Differences between current block x and
 * reference block r, each of size rows x cols, stored row-major. */
long block_sad(const int *x, const int *r, int rows, int cols)
{
    long sad = 0;
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            int d = x[i * cols + j] - r[i * cols + j];
            sad += (d < 0) ? -d : d;
        }
    }
    return sad;
}

int main(void)
{
    /* A small 2x2 demonstration (see Worked Example 3.2 in the book). */
    int x[4] = { 50, 52, 49, 51 };
    int r[4] = { 50, 50, 50, 50 };

    long sad = block_sad(x, r, 2, 2);
    printf("SAD = %ld\n", sad);   /* expected: 4 */
    return 0;
}
