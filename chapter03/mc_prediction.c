/*
 * mc_prediction.c
 * Motion-compensated prediction: given a motion vector, build the
 * prediction and output the residual.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * Motion ESTIMATION finds the vector; motion COMPENSATION uses it.
 * Compensation is cheap and runs in BOTH encoder and decoder:
 *     xhat(i,j) = ref(i + dy, j + dx)
 *     e(i,j)    = x(i,j) - xhat(i,j)
 * A good motion vector makes the residual e nearly empty.
 */

#include <stdio.h>

#define RW 32
#define RH 32
#define BS 8

/* Build the motion-compensated prediction of a BS x BS block at position
 * (by0, bx0) using motion vector (dy, dx), and write the residual. */
void mc_predict(const int *ref, int rw, int rh,
                const int *cur, int bs, int by0, int bx0,
                int dy, int dx, int *pred, int *resid)
{
    int i, j;
    for (i = 0; i < bs; i++)
        for (j = 0; j < bs; j++) {
            int ry = by0 + dy + i, rx = bx0 + dx + j;
            int p = 0;
            if (ry >= 0 && rx >= 0 && ry < rh && rx < rw)
                p = ref[ry * rw + rx];
            pred[i * bs + j]  = p;
            resid[i * bs + j] = cur[i * bs + j] - p;
        }
}

int main(void)
{
    int ref[RH * RW], cur[BS * BS], pred[BS * BS], resid[BS * BS];
    int i, j;
    long energy = 0;

    for (i = 0; i < RH; i++)
        for (j = 0; j < RW; j++)
            ref[i * RW + j] = (i * 5 + j * 2) & 0xFF;

    /* current block matches reference content displaced by (dy=2, dx=3) */
    for (i = 0; i < BS; i++)
        for (j = 0; j < BS; j++)
            cur[i * BS + j] = ref[(8 + 2 + i) * RW + (8 + 3 + j)];

    mc_predict(ref, RW, RH, cur, BS, 8, 8, 2, 3, pred, resid);

    for (i = 0; i < BS * BS; i++)
        energy += (long)resid[i] * resid[i];

    printf("residual energy = %ld (0 means a perfect match)\n", energy);
    return 0;
}
