/*
 * buffer_model.c  --  Companion code for Chapter 6: Rate Control
 * Principles of Video Coding
 *
 * A leaky-bucket buffer model. The encoder pours a variable number
 * of bits into the buffer each frame; the channel drains a fixed
 * number of bits per frame. The program tracks buffer fullness and
 * flags overflow (buffer too full) and underflow (buffer empty).
 *
 * Build:  gcc -O2 -o buffer_model buffer_model.c -lm
 * Run:    ./buffer_model
 */

#include <stdio.h>

#define N 10

int main(void)
{
    /* Bits produced by the encoder for each frame (bursty). */
    double produced[N] = {
        20000, 90000, 15000, 110000, 10000,
        85000, 30000, 95000, 12000, 8000
    };

    double drain    = 50000.0;   /* channel drains this many bits/frame */
    double capacity = 120000.0;  /* buffer size in bits                 */
    double level    = 60000.0;   /* start half full                     */

    printf("%5s %10s %10s %12s %s\n",
           "frame", "in", "out", "level", "status");
    printf("-------------------------------------------------\n");

    for (int i = 0; i < N; ++i) {
        level += produced[i];     /* encoder fills the buffer */
        const char *status = "ok";

        if (level > capacity) {   /* overflow: bits must be dropped */
            status = "OVERFLOW";
            level = capacity;
        }

        level -= drain;           /* channel drains the buffer */
        if (level < 0.0) {        /* underflow: decoder starves */
            status = "UNDERFLOW";
            level = 0.0;
        }

        printf("%5d %10.0f %10.0f %12.0f %s\n",
               i, produced[i], drain, level, status);
    }

    printf("\nRate control must keep the level between 0 and %.0f\n", capacity);
    printf("by adjusting QP so the encoder neither floods nor starves it.\n");
    return 0;
}
