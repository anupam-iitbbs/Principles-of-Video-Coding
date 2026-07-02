/*
 * simd_vs_scalar.c  --  Benchmark scalar vs SIMD-friendly filtering.
 *
 * Chapter 8: In-Loop Filtering (Principles of Video Coding, Volume 2)
 *
 * In-loop filters run on every sample, so their inner loops must vectorise.
 * This program times a scalar smoothing filter against a branchless,
 * SIMD-friendly version of the same filter. Compile with -O3 and the
 * compiler will auto-vectorise the branchless loop; the scalar loop with
 * its per-pixel branch usually cannot be vectorised as well.
 *
 *   cc -O3 -march=native simd_vs_scalar.c -o simd_vs_scalar
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N (1<<22)   /* ~4M samples */
#define ITERS 20

static int clampi(int v,int lo,int hi){ return v<lo?lo:(v>hi?hi:v); }

/* scalar: branch decides per sample whether to smooth */
static void filter_scalar(const unsigned char *in,unsigned char *out,int n){
    for(int i=1;i<n-1;i++){
        int a=in[i-1],b=in[i],c=in[i+1];
        int jump=abs(a-c);
        if(jump<32) out[i]=(unsigned char)((a+2*b+c)>>2);
        else        out[i]=in[i];
    }
}

/* branchless: compute mask arithmetically, blend both results */
static void filter_branchless(const unsigned char *in,unsigned char *out,int n){
    for(int i=1;i<n-1;i++){
        int a=in[i-1],b=in[i],c=in[i+1];
        int smoothed=(a+2*b+c)>>2;
        int mask=-(abs(a-c)<32);        /* 0xFFFF.. if true, 0 if false */
        out[i]=(unsigned char)((smoothed & mask) | (b & ~mask));
    }
}

static double bench(void (*f)(const unsigned char*,unsigned char*,int),
                    const unsigned char *in,unsigned char *out){
    clock_t t0=clock();
    for(int k=0;k<ITERS;k++) f(in,out,N);
    return (double)(clock()-t0)/CLOCKS_PER_SEC;
}

int main(void){
    unsigned char *in=malloc(N),*out=malloc(N);
    if(!in||!out){ perror("malloc"); return 1; }
    for(int i=0;i<N;i++) in[i]=(unsigned char)(clampi((i*37)%300,0,255));

    double ts=bench(filter_scalar,in,out);
    double tb=bench(filter_branchless,in,out);
    printf("scalar     : %.3f s\n", ts);
    printf("branchless : %.3f s\n", tb);
    printf("speed-up   : %.2fx\n", ts/tb);
    free(in); free(out);
    return 0;
}
