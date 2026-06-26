/*
 * weighted_prediction.c
 * Principles of Video Coding -- Chapter 4
 * Weighted prediction: xhat = w1 * P + w2 * F.  A plain average fails during a
 * brightness fade; per-reference weights restore a good prediction.  Here the
 * future reference is brighter (a fade-up), so it should carry more weight.
 */
#include <stdio.h>
#include <stdlib.h>

#define BS 4

static long sad_block(const int a[BS][BS], const int b[BS][BS]){
    long s = 0;
    for (int y = 0; y < BS; ++y)
        for (int x = 0; x < BS; ++x)
            s += labs((long)a[y][x] - b[y][x]);
    return s;
}

int main(void){
    int truth[BS][BS], P[BS][BS], F[BS][BS], avg[BS][BS], wpred[BS][BS];

    /* Fade-up: true brightness is close to the brighter future reference. */
    for (int y = 0; y < BS; ++y)
        for (int x = 0; x < BS; ++x){
            int base = 120 + 2 * x;
            truth[y][x] = base + 30;     /* bright current frame */
            P[y][x]     = base;          /* dim past reference   */
            F[y][x]     = base + 40;     /* bright future ref     */
        }

    double w1 = 0.25, w2 = 0.75;   /* weights favouring the future ref */
    for (int y = 0; y < BS; ++y)
        for (int x = 0; x < BS; ++x){
            avg[y][x]   = (P[y][x] + F[y][x] + 1) / 2;
            wpred[y][x] = (int)(w1 * P[y][x] + w2 * F[y][x] + 0.5);
        }

    printf("SAD of unweighted average     : %ld\n", sad_block(truth, avg));
    printf("SAD of weighted (0.25,0.75)   : %ld\n", sad_block(truth, wpred));
    return 0;
}
