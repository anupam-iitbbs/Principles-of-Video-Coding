/*
 * block_partition.c
 * Principles of Video Coding -- Chapter 4
 * Compare 16x16 versus 8x8 partitioning on the same content.  Smaller blocks
 * follow motion better (lower residual) but cost more motion vectors.  This
 * program reports the residual energy and the motion-vector count for each.
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define SZ 16

static int prev[SZ][SZ], cur[SZ][SZ];

/* Best-match SAD for one block over a small search range, with vector count 1. */
static long best_block_sad(int bi, int bj, int bs){
    long best = LONG_MAX;
    for (int dy = -2; dy <= 2; ++dy)
        for (int dx = -2; dx <= 2; ++dx){
            long s = 0;
            for (int y = 0; y < bs; ++y)
                for (int x = 0; x < bs; ++x){
                    int ry = bi + y + dy, rx = bj + x + dx;
                    if (ry < 0) ry = 0; if (ry > SZ-1) ry = SZ-1;
                    if (rx < 0) rx = 0; if (rx > SZ-1) rx = SZ-1;
                    s += labs((long)cur[bi+y][bj+x] - prev[ry][rx]);
                }
            if (s < best) best = s;
        }
    return best;
}

int main(void){
    for (int i = 0; i < SZ; ++i)
        for (int j = 0; j < SZ; ++j){
            prev[i][j] = (i * 11 + j * 5) & 0xFF;
            /* left half drifts by +1, right half by -2: two motions in one frame */
            int dx = (j < SZ/2) ? 1 : -2;
            int sx = j + dx; if (sx < 0) sx = 0; if (sx > SZ-1) sx = SZ-1;
            cur[i][j] = prev[i][sx];
        }

    long sad16 = best_block_sad(0, 0, 16);
    int  mv16  = 1;

    long sad8 = 0; int mv8 = 0;
    for (int bi = 0; bi < SZ; bi += 8)
        for (int bj = 0; bj < SZ; bj += 8){
            sad8 += best_block_sad(bi, bj, 8);
            mv8++;
        }

    printf("16x16 partition: residual SAD = %ld, motion vectors = %d\n", sad16, mv16);
    printf("8x8   partition: residual SAD = %ld, motion vectors = %d\n", sad8, mv8);
    return 0;
}
