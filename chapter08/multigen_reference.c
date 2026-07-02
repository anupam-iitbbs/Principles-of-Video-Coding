/*
 * multigen_reference.c  --  Why the filter must be IN the loop.
 *
 * Chapter 8: In-Loop Filtering (Principles of Video Coding, Volume 2)
 *
 * We simulate several prediction generations. Each generation predicts from
 * the previous reconstruction, adds a coding step that injects block-
 * boundary error, and stores the result as the next reference. We run two
 * worlds side by side: one that deblocks the reference each generation
 * (in-loop) and one that does not. The unfiltered world accumulates error;
 * the filtered world stays close to the original.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 64
#define BS 8
#define GENS 8

static int clampi(int v,int lo,int hi){ return v<lo?lo:(v>hi?hi:v); }

static double psnr(const int *a,const int *b,int n){
    double mse=0; for(int i=0;i<n;i++){ double d=a[i]-b[i]; mse+=d*d; }
    mse/=n; return mse==0?99.0:10.0*log10(255.0*255.0/mse);
}

/* one coding generation: quantize each block to its mean (adds seams) */
static void code_step(const int *in,int *out){
    for(int b=0;b<N;b+=BS){
        long s=0; for(int i=b;i<b+BS;i++) s+=in[i];
        int mean=(int)(s/BS);
        for(int i=b;i<b+BS;i++) out[i]=mean;
    }
}

static void deblock(int *s){
    for(int b=BS;b<N;b+=BS){
        int p0=s[b-1],q0=s[b],d=clampi((q0-p0)/4,-16,16);
        s[b-1]=clampi(p0+d,0,255);
        s[b]  =clampi(q0-d,0,255);
    }
}

int main(void){
    int orig[N];
    for(int i=0;i<N;i++) orig[i]=clampi(30+3*i,0,255);

    int refA[N],refB[N],tmp[N];
    for(int i=0;i<N;i++){ refA[i]=orig[i]; refB[i]=orig[i]; }

    printf("gen   no-filter(dB)   in-loop(dB)\n");
    for(int g=1;g<=GENS;g++){
        /* world A: no in-loop filter */
        code_step(refA,tmp);
        for(int i=0;i<N;i++) refA[i]=tmp[i];

        /* world B: filter the reference each generation */
        code_step(refB,tmp);
        for(int i=0;i<N;i++) refB[i]=tmp[i];
        deblock(refB);

        printf("%3d   %12.2f   %10.2f\n", g, psnr(orig,refA,N), psnr(orig,refB,N));
    }
    return 0;
}
