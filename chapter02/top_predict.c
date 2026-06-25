/*
 * top_predict.c  --  Volume 2, Chapter 2: Intra Prediction
 *
 * Top (vertical) intra prediction for a single block.
 * Each column is predicted from the top reference sample above it.
 *
 * Build: gcc -O2 -o top_predict top_predict.c
 */
#include <stdio.h>

#define N 4

/* top[j] is the reconstructed sample directly above column j. */
static void top_predict(const int *img, const int *top, int *resid)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            resid[i * N + j] = img[i * N + j] - top[j];
}

int main(void)
{
    int img[N * N] = {
         80, 130, 180, 60,
         81, 131, 179, 61,
         79, 129, 181, 59,
         80, 130, 180, 60
    };
    int top[N] = { 80, 130, 180, 60 };   /* reconstructed top row */
    int resid[N * N];

    top_predict(img, top, resid);

    long sse = 0;
    printf("residual block:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int e = resid[i * N + j];
            printf("%4d", e);
            sse += (long)e * e;
        }
        printf("\n");
    }
    printf("residual energy (SSE) = %ld\n", sse);
    return 0;
}
