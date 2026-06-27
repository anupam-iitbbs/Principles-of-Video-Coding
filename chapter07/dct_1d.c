/*
 * dct_1d.c -- Forward 1-D Discrete Cosine Transform (type-II), from the
 * definition. Companion code for Chapter 7, "Transform Coding".
 *
 * Demonstrates that energy lands where you expect: a constant input
 * excites only the DC coefficient; a single-frequency cosine excites a
 * single AC coefficient.
 *
 *   C(k) = a(k) * sum_{n=0}^{N-1} x(n) cos[ pi (2n+1) k / (2N) ]
 *   a(0) = sqrt(1/N),  a(k) = sqrt(2/N) for k > 0
 */
#include <stdio.h>
#include <math.h>

#define N 8

/* Forward 1-D DCT-II: in[] -> out[] */
void dct_1d(const double in[N], double out[N])
{
    for (int k = 0; k < N; k++) {
        double sum = 0.0;
        for (int n = 0; n < N; n++)
            sum += in[n] * cos(M_PI * (2 * n + 1) * k / (2.0 * N));
        double alpha = (k == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);
        out[k] = alpha * sum;
    }
}

static void show(const char *label, const double v[N])
{
    printf("%-22s", label);
    for (int i = 0; i < N; i++)
        printf("% 7.2f", v[i]);
    printf("\n");
}

int main(void)
{
    double constant[N], ramp[N], cosine[N], out[N];

    for (int n = 0; n < N; n++) {
        constant[n] = 100.0;
        ramp[n]     = (double) n;
        cosine[n]   = cos(M_PI * (2 * n + 1) * 2 / (2.0 * N)); /* k = 2 */
    }

    printf("Forward 1-D DCT of three test signals\n\n");

    dct_1d(constant, out);
    show("constant input:", out);   /* energy in DC only */

    dct_1d(ramp, out);
    show("ramp input:", out);       /* mostly low frequency */

    dct_1d(cosine, out);
    show("cosine (k=2) input:", out); /* energy in one AC bin */

    return 0;
}
