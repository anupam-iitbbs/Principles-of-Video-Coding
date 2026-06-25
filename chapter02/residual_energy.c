/*
 * residual_energy.c  --  Volume 2, Chapter 2: Intra Prediction
 *
 * Compare several intra predictors on one block by their residual
 * energy. Also computes the SAD (sum of absolute differences), the
 * cheap distortion proxy used in fast mode search.
 *
 * Build: gcc -O2 -o residual_energy residual_energy.c
 */
#include <stdio.h>

#define N 4

static long sad_of(const int *img, const int *pred)
{
    long s = 0;
    for (int k = 0; k < N * N; k++) {
        int e = img[k] - pred[k];
        s += (e < 0) ? -e : e;
    }
    return s;
}

static long sse_of(const int *img, const int *pred)
{
    long s = 0;
    for (int k = 0; k < N * N; k++) {
        int e = img[k] - pred[k];
        s += (long)e * e;
    }
    return s;
}

int main(void)
{
    /* block straddling a vertical edge: left half dark, right half bright */
    int img[N * N] = {
        40, 40, 210, 210,
        42, 38, 208, 212,
        39, 41, 211, 209,
        40, 40, 210, 210
    };
    int top[N]  = { 40, 40, 210, 210 };
    int left[N] = { 40, 42, 39, 40 };

    int dc_pred[N * N], vert_pred[N * N], horiz_pred[N * N];

    /* DC */
    int sum = 0;
    for (int k = 0; k < N; k++) sum += top[k] + left[k];
    int dc = (sum + N) / (2 * N);
    for (int k = 0; k < N * N; k++) dc_pred[k] = dc;

    /* Vertical and horizontal */
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            vert_pred[i * N + j]  = top[j];
            horiz_pred[i * N + j] = left[i];
        }

    printf("mode        SAD     SSE\n");
    printf("DC        %5ld   %5ld\n", sad_of(img, dc_pred),   sse_of(img, dc_pred));
    printf("Vertical  %5ld   %5ld\n", sad_of(img, vert_pred), sse_of(img, vert_pred));
    printf("Horizontal%5ld   %5ld\n", sad_of(img, horiz_pred),sse_of(img, horiz_pred));
    printf("\nVertical wins: it respects the vertical edge.\n");
    return 0;
}
