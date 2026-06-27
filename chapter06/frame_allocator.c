/*
 * frame_allocator.c  --  Companion code for Chapter 6: Rate Control
 * Principles of Video Coding
 *
 * Distributes a group-of-pictures (GOP) bit budget across I, P, and
 * B frames using fixed relative weights. The weights capture the
 * stable ordering I >> P > B: an I frame is coded from scratch, a P
 * frame is predicted from one reference, and a B frame from two.
 *
 * Build:  gcc -O2 -o frame_allocator frame_allocator.c -lm
 * Run:    ./frame_allocator
 */

#include <stdio.h>

/* One entry per frame in the GOP, in display order. */
static const char gop[] = { 'I', 'B', 'B', 'P', 'B', 'B', 'P', 'B' };

static double weight_of(char type)
{
    switch (type) {
        case 'I': return 20.0;   /* whole picture from scratch     */
        case 'P': return 4.0;    /* predicted from one reference   */
        case 'B': return 1.0;    /* predicted from two references  */
        default:  return 1.0;
    }
}

int main(void)
{
    int n = (int)(sizeof(gop) / sizeof(gop[0]));
    double gop_budget = 800000.0;   /* total bits for this GOP */

    double sum_w = 0.0;
    for (int i = 0; i < n; ++i) sum_w += weight_of(gop[i]);

    printf("%5s %5s %8s %12s\n", "idx", "type", "weight", "bits");
    printf("--------------------------------\n");

    for (int i = 0; i < n; ++i) {
        double w = weight_of(gop[i]);
        double bits = gop_budget * w / sum_w;
        printf("%5d %5c %8.0f %12.0f\n", i, gop[i], w, bits);
    }

    printf("\nThe I frame takes the lion's share; P and B stay lean,\n");
    printf("because each carries far less new information.\n");
    return 0;
}
