/*
 * qp_to_step.c  --  Companion code for Chapter 6: Rate Control
 * Principles of Video Coding
 *
 * Tabulates the quantization step size against the quantization
 * parameter using the standard approximation
 *
 *   Qstep is proportional to 2^(QP/6)       (Equation 6.4)
 *
 * and verifies that every +6 in QP doubles the step (and so
 * roughly halves the bitrate).
 *
 * Build:  gcc -O2 -o qp_to_step qp_to_step.c -lm
 * Run:    ./qp_to_step
 */

#include <stdio.h>
#include <math.h>

/* Quantization step as a function of QP, normalized so Qstep(0) = 1. */
static double qstep(int qp)
{
    return pow(2.0, qp / 6.0);
}

int main(void)
{
    printf("%4s %12s %14s\n", "QP", "Qstep", "ratio vs QP-6");
    printf("---------------------------------------\n");

    double prev = 0.0;
    for (int qp = 0; qp <= 51; qp += 6) {
        double q = qstep(qp);
        if (prev > 0.0)
            printf("%4d %12.4f %14.3f\n", qp, q, q / prev);
        else
            printf("%4d %12.4f %14s\n", qp, q, "-");
        prev = q;
    }

    printf("\nEach +6 in QP doubles the step (ratio ~ 2.000),\n");
    printf("which roughly halves the bits the frame produces.\n");
    return 0;
}
