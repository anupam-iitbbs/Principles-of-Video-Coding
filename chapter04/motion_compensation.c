/*
 * motion_compensation.c
 * Principles of Video Coding -- Chapter 4
 * Build a motion-compensated prediction of a frame by fetching, for each
 * block, the shifted block indicated by its motion vector from a reference
 * frame.  Prediction: xhat(i,j) = r(i+dx, j+dy).
 */
#include <stdio.h>
#include <stdlib.h>

#define W 16
#define H 16
#define BS 4   /* block size */

static int clampi(int v, int lo, int hi){
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* Fetch one BS x BS block from ref at (bi+dy, bj+dx) into pred. */
static void mc_block(const unsigned char ref[H][W], unsigned char pred[H][W],
                     int bi, int bj, int dy, int dx){
    for (int y = 0; y < BS; ++y){
        for (int x = 0; x < BS; ++x){
            int ry = clampi(bi + y + dy, 0, H - 1);
            int rx = clampi(bj + x + dx, 0, W - 1);
            pred[bi + y][bj + x] = ref[ry][rx];
        }
    }
}

int main(void){
    unsigned char ref[H][W], pred[H][W];
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            ref[i][j] = (unsigned char)((i * 13 + j * 7) & 0xFF);

    /* One motion vector per BS x BS block (here a uniform shift). */
    int dy = 1, dx = -2;
    for (int bi = 0; bi < H; bi += BS)
        for (int bj = 0; bj < W; bj += BS)
            mc_block(ref, pred, bi, bj, dy, dx);

    printf("motion-compensated prediction (mv = (%d,%d)):\n", dx, dy);
    for (int i = 0; i < H; ++i){
        for (int j = 0; j < W; ++j) printf("%4d", pred[i][j]);
        printf("\n");
    }
    return 0;
}
