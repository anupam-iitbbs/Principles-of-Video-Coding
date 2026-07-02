/*
 * edge_strength.c  --  Boundary-strength / edge-vs-artifact decision.
 *
 * Chapter 8: In-Loop Filtering (Principles of Video Coding, Volume 2)
 *
 * The heart of a deblocking filter is the decision: is this boundary
 * discontinuity a compression artifact (smooth it) or a real edge (leave
 * it)? We combine a coding-derived boundary strength with a local gradient
 * test against thresholds alpha and beta, exactly as H.264/HEVC do in
 * spirit.
 */
#include <stdio.h>
#include <stdlib.h>

/* Boundary strength from coding decisions (0 = skip, higher = filter more). */
static int boundary_strength(int left_intra,int right_intra,
                             int has_coeffs,int mv_diff){
    if(left_intra || right_intra) return 2;   /* intra edge: worst blocking */
    if(has_coeffs)               return 1;    /* residual present */
    if(mv_diff)                  return 1;    /* different motion */
    return 0;                                  /* nothing changed: skip */
}

/* Decide whether to actually filter this boundary given local samples. */
static int should_filter(int p1,int p0,int q0,int q1,
                         int bs,int alpha,int beta){
    if(bs==0) return 0;                        /* strength says skip */
    int jump  = abs(p0 - q0);
    int lvarp = abs(p1 - p0);
    int lvarq = abs(q1 - q0);
    /* big step at seam, flat interiors -> artifact signature */
    return (jump < alpha) && (lvarp < beta) && (lvarq < beta);
}

int main(void){
    struct { int p1,p0,q0,q1; const char *note; } cases[] = {
        { 64,64,80,80, "flat blocks, quantization seam (artifact)" },
        { 40,64,180,200,"strong textured edge (real edge)" },
        { 64,64,66,66, "tiny step (below threshold)" }
    };
    int alpha=20, beta=4;
    int bs = boundary_strength(1,0,1,0);   /* left block intra */
    printf("boundary strength = %d\n\n", bs);
    for(int i=0;i<3;i++){
        int f = should_filter(cases[i].p1,cases[i].p0,cases[i].q0,cases[i].q1,
                              bs,alpha,beta);
        printf("%-42s -> %s\n", cases[i].note, f?"FILTER":"skip");
    }
    return 0;
}
