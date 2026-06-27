/*
 * case_study.c  --  Companion code for Chapter 6: Rate Control
 * Principles of Video Coding
 *
 * Reproduces the worked case study from the chapter: a short clip at
 * 1080p30 targeting 4 Mbps, encoded with the GOP pattern I P P B P B.
 * For each frame we allocate bits from a type weight and a complexity
 * factor, then derive a QP from the allocation using the chapter rule
 * that +6 in QP roughly halves the bits.
 *
 * Build:  gcc -O2 -o case_study case_study.c -lm
 * Run:    ./case_study
 */

#include <stdio.h>
#include <math.h>

#define N 6

static double type_weight(char t)
{
    switch (t) {
        case 'I': return 20.0;
        case 'P': return 4.0;
        case 'B': return 1.0;
        default:  return 1.0;
    }
}

int main(void)
{
    char   type[N]       = { 'I', 'P', 'P', 'B', 'P', 'B' };
    double complexity[N] = { 2.0, 1.0, 0.8, 0.6, 1.3, 0.5 };

    double bitrate = 4.0e6;     /* 4 Mbps                 */
    double fps     = 30.0;
    double per_frame = bitrate / fps;        /* ~133,333 bits */
    double gop_budget = per_frame * N;       /* whole GOP     */

    /* Combined weight = type weight * complexity. */
    double w[N], sum_w = 0.0;
    for (int i = 0; i < N; ++i) {
        w[i] = type_weight(type[i]) * complexity[i];
        sum_w += w[i];
    }

    /* A reference QP that hits the average per-frame budget.
     * Frames given more bits than average get a lower QP, and
     * vice versa, via the +6-halves-the-bits relationship:
     *   bits = avg * 2^((QP_ref - QP)/6)
     *   ->  QP = QP_ref - 6 * log2(bits / avg)             */
    double qp_ref = 28.0;

    printf("%5s %5s %11s %12s %8s\n",
           "frame", "type", "complexity", "bits", "QP");
    printf("------------------------------------------------\n");

    double running = 0.0;
    for (int i = 0; i < N; ++i) {
        double bits = gop_budget * w[i] / sum_w;
        double qp   = qp_ref - 6.0 * log2(bits / per_frame);
        running += bits;
        printf("%5d %5c %11.1f %12.0f %8.1f\n",
               i, type[i], complexity[i], bits, qp);
    }

    printf("\nGOP target : %.0f bits   allocated : %.0f bits\n",
           gop_budget, running);
    printf("The I frame dominates; QP falls where bits rise.\n");
    return 0;
}
