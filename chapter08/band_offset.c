/*
 * band_offset.c  --  A simple band-offset (SAO-like) correction.
 *
 * Chapter 8: In-Loop Filtering (Principles of Video Coding, Volume 2)
 *
 * SAO band-offset groups samples by intensity into bands and adds a small
 * per-band offset to pull a drifted tonal range back toward the original.
 * Here we split 0..255 into 32 bands, compute the ideal offset for each
 * band from a (reconstructed, original) pair, and apply the offsets for a
 * contiguous group of four bands -- the way HEVC signals band offset.
 */
#include <stdio.h>
#include <stdlib.h>

#define N 64
#define NBANDS 32
#define BANDW  (256/NBANDS)   /* 8 levels per band */

static int clampi(int v,int lo,int hi){ return v<lo?lo:(v>hi?hi:v); }

int main(void){
    int orig[N], recon[N];
    /* a mid-tone region that reconstruction dragged down by 3 levels */
    for(int i=0;i<N;i++){
        orig[i]  = 120 + (i%8);        /* values 120..127 -> band 15 */
        recon[i] = orig[i] - 3;        /* systematic downward drift */
    }

    /* accumulate ideal offset per band = mean(orig - recon) over the band */
    long sum[NBANDS]={0}; int cnt[NBANDS]={0};
    for(int i=0;i<N;i++){
        int b = clampi(recon[i]/BANDW,0,NBANDS-1);
        sum[b]+=(orig[i]-recon[i]); cnt[b]++;
    }
    int offset[NBANDS]={0};
    for(int b=0;b<NBANDS;b++) if(cnt[b]) offset[b]=(int)(sum[b]/cnt[b]);

    /* HEVC signals offsets for a contiguous run of 4 bands. Find the busiest
       band and use it as the run start. */
    int start=0,best=0;
    for(int b=0;b<NBANDS;b++) if(cnt[b]>best){ best=cnt[b]; start=b; }

    /* apply band offset for bands [start, start+3] */
    for(int i=0;i<N;i++){
        int b = clampi(recon[i]/BANDW,0,NBANDS-1);
        if(b>=start && b<start+4)
            recon[i]=clampi(recon[i]+offset[b],0,255);
    }

    long err=0; for(int i=0;i<N;i++) err+=abs(orig[i]-recon[i]);
    printf("busiest band=%d applied offset=%d\n", start, offset[start]);
    printf("total abs error after band offset = %ld\n", err);
    return 0;
}
