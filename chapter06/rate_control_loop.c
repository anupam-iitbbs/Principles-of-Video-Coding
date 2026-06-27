/*
 * rate_control_loop.c  --  Companion code for Chapter 6: Rate Control
 * Principles of Video Coding
 *
 * A simple one-pass feedback rate controller. Each frame the encoder
 * picks a QP, the resulting bits are modeled (more complex frames and
 * lower QP cost more bits), and the controller nudges QP up or down to
 * track a target bits-per-frame. It shows how QP converges over time.
 *
 * The bit model uses the chapter relationship that +6 in QP roughly
 * halves the bits:  bits ~ complexity * 2^(-QP/6) * scale.
 *
 * Build:  gcc -O2 -o rate_control_loop rate_control_loop.c -lm
 * Run:    ./rate_control_loop
 */

#include <stdio.h>
#include <math.h>

#define N 16

/* Modeled bits for a frame at a given QP and complexity. */
static double model_bits(double complexity, double qp)
{
    double scale = 600000.0;   /* tuning constant */
    return complexity * scale * pow(2.0, -qp / 6.0);
}

int main(void)
{
    double target = 66667.0;   /* target bits/frame (1080p30 at 2 Mbps) */
    double qp     = 30.0;      /* initial guess                         */
    double qp_min = 10.0, qp_max = 51.0;

    printf("%5s %8s %12s %12s\n", "frame", "QP", "bits", "target");
    printf("------------------------------------------\n");

    for (int i = 0; i < N; ++i) {
        /* A varying but bounded complexity signal. */
        double complexity = 1.0 + 0.5 * sin(i * 0.7) + (i % 4 == 0 ? 1.0 : 0.0);

        double bits = model_bits(complexity, qp);

        printf("%5d %8.2f %12.0f %12.0f\n", i, qp, bits, target);

        /* Proportional update: if we overspent, raise QP, and vice versa.
         * The log ratio keeps the step well-behaved across a wide range. */
        double err = log2(bits / target);   /* +1 means double the target */
        qp += 6.0 * err * 0.5;              /* gentle correction          */

        if (qp < qp_min) qp = qp_min;
        if (qp > qp_max) qp = qp_max;
    }

    printf("\nQP settles near the value that makes bits track the target.\n");
    return 0;
}
