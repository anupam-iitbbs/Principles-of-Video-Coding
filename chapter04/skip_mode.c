/*
 * skip_mode.c
 * Principles of Video Coding -- Chapter 4
 * Simulate skip mode.  The block inherits a neighbour motion vector and sends
 * NO residual: the motion-compensated prediction is accepted as the final
 * reconstruction.  We check that, over a near-static region, this costs almost
 * nothing in quality.
 */
#include <stdio.h>
#include <stdlib.h>

#define BS 4

typedef struct { int x, y; } MV;

int main(void){
    int prev[16][16], cur[16][16];
    for (int i = 0; i < 16; ++i)
        for (int j = 0; j < 16; ++j){
            prev[i][j] = (i * 7 + j * 3) & 0xFF;
            cur[i][j]  = prev[i][j];          /* static background */
        }

    MV inherited = {0, 0};   /* neighbour vector reused, as in merge */
    int pred[BS][BS], recon[BS][BS];
    long residual_energy = 0;

    for (int y = 0; y < BS; ++y)
        for (int x = 0; x < BS; ++x){
            int ry = 6 + y + inherited.y, rx = 6 + x + inherited.x;
            pred[y][x]  = prev[ry][rx];
            recon[y][x] = pred[y][x];          /* skip: no residual added */
            int e = cur[6 + y][6 + x] - recon[y][x];
            residual_energy += (long)e * e;
        }

    printf("skip mode: inherited MV (%d,%d), NO residual coded\n",
           inherited.x, inherited.y);
    printf("reconstruction error energy = %ld\n", residual_energy);
    printf("bits sent for this block: essentially one skip flag\n");
    return 0;
}
