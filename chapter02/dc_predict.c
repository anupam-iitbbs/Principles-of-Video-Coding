/*
 * dc_predict.c  --  Volume 2, Chapter 2: Intra Prediction
 *
 * DC intra prediction: fill the block with the average of the
 * available top and left reference samples.
 *
 * Build: gcc -O2 -o dc_predict dc_predict.c
 */
#include <stdio.h>

#define N 4

/* Average of the N top and N left reference samples, rounded to nearest. */
static int dc_value(const int *top, const int *left)
{
    int sum = 0;
    for (int k = 0; k < N; k++) sum += top[k] + left[k];
    return (sum + N) / (2 * N);   /* rounded DC value */
}

int main(void)
{
    int img[N * N] = {
        119, 121, 120, 122,
        120, 118, 121, 119,
        122, 120, 119, 121,
        118, 121, 120, 120
    };
    int top[N]  = { 120, 122, 119, 121 };
    int left[N] = { 118, 121, 120, 123 };

    int dc = dc_value(top, left);
    long sad = 0;
    printf("DC prediction value = %d\n", dc);
    printf("residual block:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int e = img[i * N + j] - dc;
            printf("%4d", e);
            sad += (e < 0) ? -e : e;
        }
        printf("\n");
    }
    printf("SAD = %ld\n", sad);
    return 0;
}
