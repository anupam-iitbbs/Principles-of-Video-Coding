/*
 * basis_visualizer.c
 * Companion code for Chapter 5: Transform Coding (Volume 2)
 *
 * Builds the 1-D DCT basis vectors and numerically verifies their
 * orthogonality by computing all pairwise dot products. Off-diagonal
 * products should be ~0; diagonal products ~1 for an orthonormal basis.
 */

#include <stdio.h>
#include <math.h>

#define N 8

static double basis[N][N];

static void build_basis(void) {
    for (int u = 0; u < N; ++u) {
        double alpha = (u == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);
        for (int n = 0; n < N; ++n)
            basis[u][n] = alpha * cos((2.0 * n + 1.0) * u * M_PI / (2.0 * N));
    }
}

static double dot(const double *a, const double *b) {
    double s = 0.0;
    for (int i = 0; i < N; ++i) s += a[i] * b[i];
    return s;
}

int main(void) {
    build_basis();
    printf("Pairwise dot products (should be ~1 on diagonal, ~0 off):\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            double d = dot(basis[i], basis[j]);
            printf("% .3f ", fabs(d) < 1e-12 ? 0.0 : d);
        }
        printf("\n");
    }
    return 0;
}
