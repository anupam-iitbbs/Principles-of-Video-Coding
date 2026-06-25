/*
 * left_predict.c  --  Volume 2, Chapter 2: Intra Prediction
 *
 * Left (horizontal) intra prediction for a single block.
 * Each pixel is predicted from its reconstructed left neighbor;
 * the leftmost column is predicted from the left reference column.
 *
 * Build: gcc -O2 -o left_predict left_predict.c
 */
#include <stdio.h>
#include <stdlib.h>

#define N 4   /* block size (N x N) */

/* Predict an N x N block by copying the left reference column across each row.
 * left[i] is the reconstructed sample immediately to the left of row i.
 * pred and resid are N*N arrays in row-major order; img is the true block. */
static void left_predict(const int *img, const int *left, int *pred, int *resid)
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            pred[i * N + j]  = left[i];                 /* horizontal mode */
            resid[i * N + j] = img[i * N + j] - pred[i * N + j];
        }
    }
}

int main(void)
{
    /* A small block that is roughly constant across each row. */
    int img[N * N] = {
        100, 101,  99, 100,
        150, 151, 149, 150,
        200, 199, 201, 200,
         60,  61,  59,  60
    };
    int left[N] = { 100, 150, 200, 60 };   /* reconstructed left neighbors */
    int pred[N * N], resid[N * N];

    left_predict(img, left, pred, resid);

    long sad = 0;
    printf("residual block (e = x - x_hat):\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int e = resid[i * N + j];
            printf("%4d", e);
            sad += (e < 0) ? -e : e;
        }
        printf("\n");
    }
    printf("SAD = %ld\n", sad);
    return 0;
}
