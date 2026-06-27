/*
 * complexity_estimate.c  --  Companion code for Chapter 6: Rate Control
 * Principles of Video Coding
 *
 * Estimates a per-frame complexity score from residual energy, then
 * uses the scores to drive a needs-based bit allocation. The residual
 * energy is the sum of squared residual samples; frames with more
 * leftover structure after prediction score higher and earn more bits.
 *
 * Build:  gcc -O2 -o complexity_estimate complexity_estimate.c -lm
 * Run:    ./complexity_estimate
 */

#include <stdio.h>

#define F 5          /* number of frames                 */
#define B 8          /* residual samples per frame (toy) */

/* Sum of squared residuals: a simple complexity proxy. */
static double residual_energy(const int *res, int n)
{
    double e = 0.0;
    for (int i = 0; i < n; ++i) e += (double)res[i] * res[i];
    return e;
}

int main(void)
{
    /* A toy residual block for each frame. */
    int res[F][B] = {
        {  1,  0, -1,  0,  1,  0,  0, -1 },   /* near-flat: easy */
        { 12, -9, 14, -7, 10, -11, 8, -6 },   /* busy: hard      */
        {  3, -2,  4, -1,  2,  0,  1, -2 },   /* moderate        */
        { 20, 18, -22, 17, -19, 21, -16, 15 },/* very busy       */
        {  2,  1, -1,  0,  1, -1,  0,  1 },    /* near-flat: easy */
    };

    double score[F], total = 0.0;
    for (int f = 0; f < F; ++f) {
        score[f] = residual_energy(res[f], B);
        total += score[f];
    }

    double budget = 500000.0;   /* bits to share across the frames */
    printf("%5s %14s %12s\n", "frame", "complexity", "bits");
    printf("---------------------------------\n");
    for (int f = 0; f < F; ++f) {
        double bits = budget * score[f] / total;
        printf("%5d %14.0f %12.0f\n", f, score[f], bits);
    }

    printf("\nBits follow residual energy: busy frames earn more,\n");
    printf("flat frames earn less, all from one cheap measurement.\n");
    return 0;
}
