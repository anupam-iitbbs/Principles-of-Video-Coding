/*
 * lambda_decision.c  --  Companion code for Chapter 6: Rate Control
 * Principles of Video Coding
 *
 * Lagrangian mode decision. Given a small set of candidate codings,
 * each with a known distortion D and bit cost R, the encoder picks
 * the candidate that minimizes
 *
 *   J = D + lambda * R          (Equation 6.3)
 *
 * The chosen candidate shifts as lambda changes: small lambda favors
 * low distortion (spend bits freely); large lambda favors low rate.
 *
 * Build:  gcc -O2 -o lambda_decision lambda_decision.c -lm
 * Run:    ./lambda_decision
 */

#include <stdio.h>

#define M 4

int main(void)
{
    const char *name[M] = { "skip", "coarse", "medium", "fine" };
    double D[M] = { 900.0, 400.0, 150.0, 40.0 };   /* distortion */
    double R[M] = { 10.0,  120.0, 380.0, 900.0 };  /* bits       */

    double lambdas[] = { 0.1, 0.5, 1.0, 3.0, 10.0 };
    int nl = (int)(sizeof(lambdas) / sizeof(lambdas[0]));

    printf("%8s %10s\n", "lambda", "chosen");
    printf("--------------------\n");

    for (int k = 0; k < nl; ++k) {
        double lambda = lambdas[k];
        int best = 0;
        double best_J = D[0] + lambda * R[0];
        for (int i = 1; i < M; ++i) {
            double J = D[i] + lambda * R[i];
            if (J < best_J) { best_J = J; best = i; }
        }
        printf("%8.2f %10s\n", lambda, name[best]);
    }

    printf("\nAs lambda grows, bits become precious and the encoder\n");
    printf("retreats from the fine, expensive coding modes.\n");
    return 0;
}
