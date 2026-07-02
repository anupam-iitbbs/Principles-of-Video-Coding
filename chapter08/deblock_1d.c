/*
 * deblock_1d.c  --  A minimal 1D deblocking filter.
 *
 * Chapter 8: In-Loop Filtering (Principles of Video Coding, Volume 2)
 *
 * A synthetic 1D signal contains two flat blocks that quantization has
 * pushed apart, creating a step at the block boundary. We soften the step
 * ONLY when the block interiors are flat (artifact signature). A clipped
 * correction guarantees we never overshoot into the neighbouring block.
 */
#include <stdio.h>
#include <stdlib.h>

static int clampi(int v, int lo, int hi){ return v<lo?lo:(v>hi?hi:v); }

/* Filter the boundary between s[i-1] (=p0) and s[i] (=q0).
   alpha: max boundary jump we will still filter; beta: max interior variation. */
static void deblock_boundary(int *s, int i, int alpha, int beta){
    int p1=s[i-2], p0=s[i-1], q0=s[i], q1=s[i+1];
    int jump = abs(p0 - q0);
    int inner = abs(p1 - p0) + abs(q1 - q0);
    if(jump < alpha && inner < beta){
        int delta = (q0 - p0) / 4;
        delta = clampi(delta, -(jump/2), (jump/2));
        s[i-1] = clampi(p0 + delta, 0, 255);
        s[i]   = clampi(q0 - delta, 0, 255);
    }
}

int main(void){
    /* two flat blocks of width 4, boundary at index 4 */
    int s[8] = {64,64,64,64, 80,80,80,80};
    printf("before:");
    for(int i=0;i<8;i++) printf(" %d", s[i]);
    printf("\n");

    deblock_boundary(s, 4, /*alpha=*/20, /*beta=*/4);

    printf("after :");
    for(int i=0;i<8;i++) printf(" %d", s[i]);
    printf("\n");
    return 0;
}
