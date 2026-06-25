/*
 * vertical_predict.c  --  Volume 2, Chapter 2: Intra Prediction
 *
 * Pure vertical intra prediction (the canonical directional mode):
 * every pixel in a column takes the value of the single top reference
 * sample above that column. Demonstrates that the prediction is
 * independent of the row index.
 *
 * Build: gcc -O2 -o vertical_predict vertical_predict.c
 */
#include <stdio.h>

#define N 8

static void vertical_predict(const int *top, int *pred)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            pred[i * N + j] = top[j];   /* copy column-wise, row independent */
}

int main(void)
{
    int top[N] = { 50, 60, 70, 80, 90, 100, 110, 120 };
    int pred[N * N];

    vertical_predict(top, pred);

    printf("vertical prediction (each column == its top sample):\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%4d", pred[i * N + j]);
        printf("\n");
    }
    return 0;
}
