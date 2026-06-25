/*
 * motion_vector.c
 * Given a block and a reference frame, return the best motion vector
 * (dx, dy) and its distortion.
 *
 * Companion code for Chapter 3: Motion Estimation.
 * Principles of Video Coding (Volume 2).
 *
 * This is a thin wrapper around a SAD-based search that emphasises the
 * OUTPUT of motion estimation: the displacement (dx, dy) that the encoder
 * will signal to the decoder.  The motion vector points from the current
 * block to its best match in the reference frame.
 */

#include <stdio.h>
#include <limits.h>

typedef struct {
    int dx;       /* horizontal displacement */
    int dy;       /* vertical displacement   */
    long sad;     /* distortion at that displacement */
} MotionVector;

/* Search the reference frame 'ref' (rw x rh) for the best match of the
 * block 'cur' (bs x bs) whose own position is (by0, bx0).  The search
 * range is +/- p in each direction. */
MotionVector estimate_mv(const int *ref, int rw, int rh,
                         const int *cur, int bs,
                         int by0, int bx0, int p)
{
    MotionVector mv = { 0, 0, LONG_MAX };
    int dy, dx, i, j;
    for (dy = -p; dy <= p; dy++) {
        for (dx = -p; dx <= p; dx++) {
            int ry = by0 + dy, rx = bx0 + dx;
            long sad = 0;
            if (ry < 0 || rx < 0 || ry + bs > rh || rx + bs > rw)
                continue;
            for (i = 0; i < bs; i++)
                for (j = 0; j < bs; j++) {
                    int d = cur[i * bs + j] - ref[(ry + i) * rw + (rx + j)];
                    sad += (d < 0) ? -d : d;
                }
            if (sad < mv.sad) { mv.sad = sad; mv.dx = dx; mv.dy = dy; }
        }
    }
    return mv;
}

int main(void)
{
    enum { RW = 32, RH = 32, BS = 4 };
    int ref[RH * RW];
    int cur[BS * BS];
    int i, j;

    for (i = 0; i < RH; i++)
        for (j = 0; j < RW; j++)
            ref[i * RW + j] = (i * 5 + j * 2) & 0xFF;

    /* current block equals reference content shifted by (dy=2, dx=1) */
    for (i = 0; i < BS; i++)
        for (j = 0; j < BS; j++)
            cur[i * BS + j] = ref[(10 + 2 + i) * RW + (10 + 1 + j)];

    MotionVector mv = estimate_mv(ref, RW, RH, cur, BS, 10, 10, 7);
    printf("MV = (dx=%d, dy=%d), SAD = %ld\n", mv.dx, mv.dy, mv.sad);
    return 0;
}
