/*
 * biprediction.c
 * Principles of Video Coding -- Chapter 4
 * Bidirectional prediction.  Average a past prediction P and a future
 * prediction F to form a B-block prediction: xhat = (P + F) / 2.  Averaging
 * two predictions reduces the error below that of either one alone.
 */
#include <stdio.h>
#include <stdlib.h>

#define BS 4

int main(void){
    int truth[BS][BS], P[BS][BS], F[BS][BS], bi[BS][BS];

    for (int y = 0; y < BS; ++y)
        for (int x = 0; x < BS; ++x){
            truth[y][x] = 100 + 4 * y + 3 * x;
            P[y][x] = truth[y][x] + ((x + y) % 2 ? 5 : -4);   /* past, biased */
            F[y][x] = truth[y][x] + ((x + y) % 2 ? -6 : 3);   /* future, biased */
        }

    long eP = 0, eF = 0, eB = 0;
    for (int y = 0; y < BS; ++y)
        for (int x = 0; x < BS; ++x){
            bi[y][x] = (P[y][x] + F[y][x] + 1) / 2;   /* rounded average */
            eP += labs((long)truth[y][x] - P[y][x]);
            eF += labs((long)truth[y][x] - F[y][x]);
            eB += labs((long)truth[y][x] - bi[y][x]);
        }

    printf("SAD of past-only   prediction P : %ld\n", eP);
    printf("SAD of future-only prediction F : %ld\n", eF);
    printf("SAD of bidirectional (P+F)/2     : %ld\n", eB);
    return 0;
}
