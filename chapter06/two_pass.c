/*
 * two_pass.c  --  Companion code for Chapter 6: Rate Control
 * Principles of Video Coding
 *
 * Contrasts one-pass and two-pass bit allocation.
 *   Pass 1: encode (or analyze) every frame to record its complexity.
 *   Pass 2: allocate the whole budget with hindsight, in proportion
 *           to the recorded complexities.
 * One-pass, by contrast, must guess each frame from the past alone.
 * We model one-pass with a lagged estimate of complexity and compare
 * how well each scheme matches bits to the true difficulty.
 *
 * Build:  gcc -O2 -o two_pass two_pass.c -lm
 * Run:    ./two_pass
 */

#include <stdio.h>
#include <math.h>

#define N 8

int main(void)
{
    /* True (but, to a one-pass encoder, unknown ahead of time) complexity. */
    double truth[N] = { 1.0, 1.0, 5.0, 5.0, 1.0, 4.0, 1.0, 2.0 };
    double budget = 800000.0;

    /* ---- Pass 1: measure total complexity ---- */
    double total = 0.0;
    for (int i = 0; i < N; ++i) total += truth[i];

    /* ---- Two-pass allocation: proportional, with full knowledge ---- */
    /* ---- One-pass allocation: use previous frame as the estimate ---- */
    double err1 = 0.0, err2 = 0.0;
    double prev = truth[0];

    printf("%5s %8s %12s %12s\n", "frame", "truth", "1-pass", "2-pass");
    printf("------------------------------------------\n");

    for (int i = 0; i < N; ++i) {
        double two = budget * truth[i] / total;        /* hindsight   */
        double one = budget * prev / total;            /* lagged guess */

        /* Mismatch between bits and true need (lower is better). */
        double ideal = budget * truth[i] / total;
        err1 += fabs(one - ideal);
        err2 += fabs(two - ideal);

        printf("%5d %8.1f %12.0f %12.0f\n", i, truth[i], one, two);
        prev = truth[i];
    }

    printf("\nTotal allocation error  one-pass: %.0f   two-pass: %.0f\n", err1, err2);
    printf("Two-pass matches bits to difficulty almost perfectly,\n");
    printf("because it has already seen the whole sequence.\n");
    return 0;
}
