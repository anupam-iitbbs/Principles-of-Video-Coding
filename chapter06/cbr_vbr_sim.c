/*
 * cbr_vbr_sim.c  --  Companion code for Chapter 6: Rate Control
 * Principles of Video Coding
 *
 * Feeds a sequence of frame complexities through two allocators:
 *   - CBR: every frame receives the same number of bits.
 *   - VBR: bits are distributed in proportion to complexity, so
 *          perceived quality stays roughly even.
 * It then reports the resulting bits and a crude quality proxy
 * (bits delivered relative to bits demanded) for each frame.
 *
 * Build:  gcc -O2 -o cbr_vbr_sim cbr_vbr_sim.c -lm
 * Run:    ./cbr_vbr_sim
 */

#include <stdio.h>

#define N 8

int main(void)
{
    /* Relative complexity of each frame (higher = harder to code). */
    double complexity[N] = { 1.0, 4.0, 1.5, 5.0, 1.0, 3.0, 2.0, 1.0 };

    double total_bits = 800000.0;          /* total budget for the run */
    double cbr_bits   = total_bits / N;    /* equal share per frame    */

    double sum_c = 0.0;
    for (int i = 0; i < N; ++i) sum_c += complexity[i];

    printf("%5s %10s %12s %12s %10s %10s\n",
           "frame", "complex", "CBR bits", "VBR bits",
           "CBR qual", "VBR qual");
    printf("------------------------------------------------------------------\n");

    for (int i = 0; i < N; ++i) {
        double vbr_bits = total_bits * complexity[i] / sum_c;

        /* Quality proxy: bits delivered per unit of complexity.
         * VBR holds this roughly constant; CBR does not.        */
        double cbr_q = cbr_bits / complexity[i];
        double vbr_q = vbr_bits / complexity[i];

        printf("%5d %10.1f %12.0f %12.0f %10.0f %10.0f\n",
               i, complexity[i], cbr_bits, vbr_bits, cbr_q, vbr_q);
    }

    printf("\nCBR keeps bits flat, so quality swings with complexity.\n");
    printf("VBR varies bits, so quality (bits per complexity) stays even.\n");
    return 0;
}
