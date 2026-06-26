/*
 * residual.c
 * Principles of Video Coding -- Chapter 4
 * Compute the prediction residual e = x - xhat for a block, and report its
 * energy.  After a good motion-compensated prediction the residual clusters
 * near zero, which is exactly what makes the later transform stage cheap.
 */
#include <stdio.h>
#include <stdlib.h>

#define N 8

int main(void){
    int x[N][N], xhat[N][N], e[N][N];

    /* Current block and a close prediction of it. */
    for (int i = 0; i < N; ++i){
        for (int j = 0; j < N; ++j){
            x[i][j]    = 100 + i + j;
            xhat[i][j] = 100 + i + j + ((i + j) % 3 - 1); /* small offset */
        }
    }

    long energy = 0, absum = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j){
            e[i][j] = x[i][j] - xhat[i][j];
            energy += (long)e[i][j] * e[i][j];
            absum  += abs(e[i][j]);
        }

    printf("residual e = x - xhat:\n");
    for (int i = 0; i < N; ++i){
        for (int j = 0; j < N; ++j) printf("%4d", e[i][j]);
        printf("\n");
    }
    printf("\nSSD (energy) = %ld\n", energy);
    printf("SAD (abs sum) = %ld\n", absum);
    return 0;
}
