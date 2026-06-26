/*
 * projection.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * Projects a signal onto an orthonormal basis using the dot product,
 * then reconstructs the signal from its coefficients to show that an
 * orthonormal projection loses no information.
 */

#include <stdio.h>

#define N 4

/* Dot product of two length-N vectors. */
static double dot(const double *a, const double *b) {
    double s = 0.0;
    for (int i = 0; i < N; ++i) s += a[i] * b[i];
    return s;
}

int main(void) {
    /* A signal and a simple orthonormal basis (the identity axes). */
    double x[N]     = { 4.0, 2.0, 7.0, 1.0 };
    double basis[N][N] = {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    };

    /* Forward: coefficient c_k = x . b_k. */
    double c[N];
    for (int k = 0; k < N; ++k) c[k] = dot(x, basis[k]);

    /* Inverse: rebuild x as a weighted sum of basis vectors. */
    double xr[N] = { 0 };
    for (int k = 0; k < N; ++k)
        for (int i = 0; i < N; ++i)
            xr[i] += c[k] * basis[k][i];

    printf("coefficients:");
    for (int k = 0; k < N; ++k) printf(" %.2f", c[k]);
    printf("\nreconstruction:");
    for (int i = 0; i < N; ++i) printf(" %.2f", xr[i]);
    printf("\n");
    return 0;
}
