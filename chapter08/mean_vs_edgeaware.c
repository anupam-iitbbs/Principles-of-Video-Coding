/*
 * mean_vs_edgeaware.c  --  Mean filter vs edge-aware filter.
 *
 * Chapter 8: In-Loop Filtering (Principles of Video Coding, Volume 2)
 *
 * A plain mean (box) filter suppresses noise but also blurs real edges.
 * An edge-aware filter weights neighbours by how similar they are to the
 * centre sample, so it smooths flat regions while preserving edges. We
 * run both on a 1D signal with a step edge plus noise and report how much
 * of the edge each one preserves.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 16

static double mean3(const int *s,int i){
    return (s[i-1]+s[i]+s[i+1])/3.0;
}

/* range-weighted (bilateral-like) filter: weight falls off with intensity
   difference, controlled by sigma. */
static double edgeaware3(const int *s,int i,double sigma){
    double wsum=0,acc=0;
    for(int k=-1;k<=1;k++){
        double diff=s[i+k]-s[i];
        double w=exp(-(diff*diff)/(2*sigma*sigma));
        wsum+=w; acc+=w*s[i+k];
    }
    return acc/wsum;
}

int main(void){
    /* flat 50 then step up to 200, with a little noise */
    int s[N]={50,51,49,50,52,48,50,51, 200,199,201,200,198,202,200,199};
    double m[N],e[N];
    for(int i=1;i<N-1;i++){ m[i]=mean3(s,i); e[i]=edgeaware3(s,i,10.0); }

    int edge=7; /* boundary between the two levels */
    double true_step  = fabs((double)s[edge+1]-s[edge]);
    double mean_step  = fabs(m[edge+1]-m[edge]);
    double ea_step    = fabs(e[edge+1]-e[edge]);

    printf("true edge step      : %.1f\n", true_step);
    printf("mean filter step    : %.1f (edge blurred)\n", mean_step);
    printf("edge-aware step     : %.1f (edge preserved)\n", ea_step);
    return 0;
}
