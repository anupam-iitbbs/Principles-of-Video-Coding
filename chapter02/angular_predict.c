/*
 * angular_predict.c  --  Volume 2, Chapter 2: Intra Prediction
 *
 * A single angular intra mode. For each row we project the reference
 * samples into the block along a fixed slope and interpolate between
 * the two nearest reference samples (sub-sample accuracy).
 *
 * The reference array 'ref' holds the extended top row:
 * ref[0..N-1] are the samples above columns 0..N-1, and ref[N..2N-1]
 * are the top-right extension used when the projection runs past the
 * block width.
 *
 * Build: gcc -O2 -o angular_predict angular_predict.c
 */
#include <stdio.h>

#define N 4

/* 'slope' is the horizontal shift per row, in 1/32-sample units,
 * the same fixed-point convention real codecs use. */
static void angular_predict(const int *ref, int slope, int *pred)
{
    for (int i = 0; i < N; i++) {
        int offset = (i + 1) * slope;   /* accumulated projection, in 1/32 units */
        for (int j = 0; j < N; j++) {
            int pos   = (j << 5) + offset;   /* position in 1/32 units */
            int idx   = pos >> 5;            /* integer reference index */
            int frac  = pos & 31;            /* fractional part (0..31) */
            int a = ref[idx];
            int b = ref[idx + 1];
            /* linear interpolation between the two nearest reference samples */
            pred[i * N + j] = ((32 - frac) * a + frac * b + 16) >> 5;
        }
    }
}

int main(void)
{
    /* extended top reference row (length 2N) */
    int ref[2 * N] = { 40, 60, 80, 100, 120, 140, 160, 180 };
    int pred[N * N];
    int slope = 8;   /* ~ shift of 1/4 sample per row */

    angular_predict(ref, slope, pred);

    printf("angular prediction (slope = %d / 32 per row):\n", slope);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%4d", pred[i * N + j]);
        printf("\n");
    }
    return 0;
}
