/*
 * zigzag_scan.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * A preview of the zig-zag scan that orders 2-D transform coefficients
 * from low frequency to high frequency. The full role of this ordering
 * appears with quantization and entropy coding in later chapters; here
 * we simply plant the seed by reading an 8x8 block in zig-zag order.
 */

#include <stdio.h>

#define N 8

/* Standard JPEG zig-zag order for an 8x8 block. */
static const int zigzag[N * N] = {
     0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

int main(void) {
    /* Fill a block with its raster index so the scan order is visible. */
    int block[N][N];
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) block[i][j] = i * N + j;

    printf("Coefficients read in zig-zag (low to high frequency):\n");
    for (int k = 0; k < N * N; ++k) {
        int idx = zigzag[k];
        printf("%3d", block[idx / N][idx % N]);
        if ((k + 1) % N == 0) printf("\n");
    }
    return 0;
}
