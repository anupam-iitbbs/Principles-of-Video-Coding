/*
 * merge_mode.c
 * Principles of Video Coding -- Chapter 4
 * Simulate merge mode.  Build a small candidate list from neighbour motion
 * vectors, pick the candidate that best predicts the current block, and code
 * only its index -- no motion vector difference is sent.
 */
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define BS 4
#define MAXC 4

typedef struct { int x, y; } MV;

/* SAD between current block and a reference block shifted by mv. */
static long block_sad(const int cur[BS][BS], const int ref[16][16],
                      int bi, int bj, MV mv){
    long s = 0;
    for (int y = 0; y < BS; ++y)
        for (int x = 0; x < BS; ++x){
            int ry = bi + y + mv.y, rx = bj + x + mv.x;
            if (ry < 0) ry = 0; if (ry > 15) ry = 15;
            if (rx < 0) rx = 0; if (rx > 15) rx = 15;
            s += labs((long)cur[y][x] - ref[ry][rx]);
        }
    return s;
}

int main(void){
    int ref[16][16];
    for (int i = 0; i < 16; ++i)
        for (int j = 0; j < 16; ++j)
            ref[i][j] = (i * 9 + j * 5) & 0xFF;

    /* Current block equals the reference shifted by the true motion (2,1). */
    int cur[BS][BS];
    MV truth = {2, 1};
    for (int y = 0; y < BS; ++y)
        for (int x = 0; x < BS; ++x)
            cur[y][x] = ref[4 + y + truth.y][4 + x + truth.x];

    MV cand[MAXC] = { {0,0}, {2,1}, {3,0}, {1,1} }; /* neighbour candidates */
    int best = 0; long bestsad = LONG_MAX;
    for (int k = 0; k < MAXC; ++k){
        long s = block_sad(cur, ref, 4, 4, cand[k]);
        printf("candidate %d (%d,%d): SAD = %ld\n", k, cand[k].x, cand[k].y, s);
        if (s < bestsad){ bestsad = s; best = k; }
    }
    printf("\nmerge index = %d  -> reuse MV (%d,%d), no MVD sent\n",
           best, cand[best].x, cand[best].y);
    return 0;
}
